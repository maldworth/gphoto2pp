#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_file_type_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

enum class file_read_result { success, io_error, oversized };

file_read_result next_file_read_result = file_read_result::success;
std::string camera_file_data = "camera-data";
std::string requested_folder;
std::string requested_file;
gphoto2::CameraFileType requested_type = gphoto2::GP_FILE_TYPE_NORMAL;
std::uint64_t requested_offset = 0;
std::uint64_t requested_size = 0;
int file_read_calls = 0;
gphoto2::GPPortInfo fake_port = nullptr;

}  // namespace

namespace gphoto2 {

extern "C" int gp_camera_init(Camera *, GPContext *) {
    return GP_OK;
}

extern "C" int gp_camera_get_port_info(Camera *, GPPortInfo *info) {
    *info = fake_port;
    return GP_OK;
}

extern "C" int gp_camera_get_abilities(Camera *, CameraAbilities *abilities) {
    std::memset(abilities, 0, sizeof(*abilities));
    std::snprintf(abilities->model, sizeof(abilities->model), "%s", "Synthetic camera");
    return GP_OK;
}

extern "C" int gp_camera_file_read(Camera *, const char *folder,
    const char *file, CameraFileType type, std::uint64_t offset, char *buffer,
    std::uint64_t *size, GPContext *) {
    ++file_read_calls;
    requested_folder = folder;
    requested_file = file;
    requested_type = type;
    requested_offset = offset;
    requested_size = *size;

    if (next_file_read_result == file_read_result::io_error) {
        return GP_ERROR_IO;
    }
    if (next_file_read_result == file_read_result::oversized) {
        *size += 1;
        return GP_OK;
    }
    if (offset >= camera_file_data.size()) {
        *size = 0;
        return GP_OK;
    }

    const auto available = static_cast<std::uint64_t>(
        camera_file_data.size() - static_cast<std::size_t>(offset));
    const auto count = std::min(*size, available);
    std::memcpy(buffer, camera_file_data.data() + offset,
        static_cast<std::size_t>(count));
    *size = count;
    return GP_OK;
}

}  // namespace gphoto2

class CameraWrapper_FileRead : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;
    std::unique_ptr<gphoto2pp::CameraWrapper> m_camera;

public:
    void setUp() {
        next_file_read_result = file_read_result::success;
        requested_folder.clear();
        requested_file.clear();
        requested_offset = 0;
        requested_size = 0;
        file_read_calls = 0;
        if (gphoto2::gp_port_info_list_new(&m_ports) != GP_OK ||
            gphoto2::gp_port_info_new(&fake_port) != GP_OK ||
            gphoto2::gp_port_info_set_name(fake_port, "Synthetic port") != GP_OK ||
            gphoto2::gp_port_info_set_type(fake_port, gphoto2::GP_PORT_USB) != GP_OK ||
            gphoto2::gp_port_info_set_path(fake_port, "fake:no-usb") != GP_OK ||
            gphoto2::gp_port_info_list_append(m_ports, fake_port) < GP_OK) {
            throw std::runtime_error("Unable to configure synthetic port info");
        }
        m_camera.reset(new gphoto2pp::CameraWrapper());
    }

    void tearDown() {
        m_camera.reset();
        if (m_ports != nullptr) {
            gphoto2::gp_port_info_list_free(m_ports);
            m_ports = nullptr;
        }
        fake_port = nullptr;
    }

    void testReadsRequestedRangeAndShortReads() {
        const auto data = m_camera->fileRead("/DCIM", "image.jpg",
            gphoto2pp::CameraFileTypeWrapper::Normal, 2, 4);
        TS_ASSERT_EQUALS(std::string(data.begin(), data.end()), "mera");
        TS_ASSERT_EQUALS(requested_folder, "/DCIM");
        TS_ASSERT_EQUALS(requested_file, "image.jpg");
        TS_ASSERT_EQUALS(requested_type, gphoto2::GP_FILE_TYPE_NORMAL);
        TS_ASSERT_EQUALS(requested_offset, 2);
        TS_ASSERT_EQUALS(requested_size, 4);

        const auto short_read = m_camera->fileRead("/DCIM", "image.jpg",
            gphoto2pp::CameraFileTypeWrapper::Normal, 8, 10);
        TS_ASSERT_EQUALS(std::string(short_read.begin(), short_read.end()), "ata");
        const auto at_end = m_camera->fileRead("/DCIM", "image.jpg",
            gphoto2pp::CameraFileTypeWrapper::Normal, 20, 10);
        TS_ASSERT(at_end.empty());
    }

    void testZeroSizeAndEmbeddedNulValidation() {
        const int calls = file_read_calls;
        TS_ASSERT(m_camera->fileRead("/DCIM", "image.jpg",
            gphoto2pp::CameraFileTypeWrapper::Normal, 0, 0).empty());
        TS_ASSERT_EQUALS(file_read_calls, calls);

        bool rejected = false;
        try {
            m_camera->fileRead(std::string("/DCIM\0other", 11), "image.jpg",
                gphoto2pp::CameraFileTypeWrapper::Normal, 0, 1);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_read_calls, calls);

        rejected = false;
        try {
            m_camera->fileRead("/DCIM", "image.jpg",
                gphoto2pp::CameraFileTypeWrapper::Normal, 0,
                std::numeric_limits<std::uint64_t>::max());
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_read_calls, calls);
    }

    void testFileReadErrorsAndMalformedLength() {
        next_file_read_result = file_read_result::io_error;
        bool preserved_error = false;
        try {
            m_camera->fileRead("/DCIM", "image.jpg",
                gphoto2pp::CameraFileTypeWrapper::Normal, 0, 4);
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(preserved_error);

        next_file_read_result = file_read_result::oversized;
        bool rejected_length = false;
        try {
            m_camera->fileRead("/DCIM", "image.jpg",
                gphoto2pp::CameraFileTypeWrapper::Normal, 0, 4);
        } catch (const std::runtime_error &) {
            rejected_length = true;
        }
        TS_ASSERT(rejected_length);
    }
};
