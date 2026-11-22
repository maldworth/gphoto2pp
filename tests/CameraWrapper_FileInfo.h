#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_file_info.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

enum class file_info_result {
    populated,
    partial,
    unsupported,
    io_error,
    bad_file_type,
    bad_preview_type,
    bad_audio_type
};

file_info_result next_file_info_result = file_info_result::populated;
std::string requested_folder;
std::string requested_file;
int file_info_calls = 0;
gphoto2::CameraFileInfo last_file_update{};
int file_update_calls = 0;
int file_update_result = GP_OK;
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

extern "C" int gp_camera_file_get_info(
    Camera *, const char *folder, const char *file, CameraFileInfo *info,
    GPContext *) {
    ++file_info_calls;
    requested_folder = folder;
    requested_file = file;
    if (next_file_info_result == file_info_result::unsupported) {
        return GP_ERROR_NOT_SUPPORTED;
    }
    if (next_file_info_result == file_info_result::io_error) {
        return GP_ERROR_IO;
    }

    std::memset(info, 0, sizeof(*info));
    info->file.fields = static_cast<CameraFileInfoFields>(
        GP_FILE_INFO_TYPE | GP_FILE_INFO_SIZE | GP_FILE_INFO_WIDTH |
        GP_FILE_INFO_HEIGHT | GP_FILE_INFO_PERMISSIONS | GP_FILE_INFO_STATUS |
        GP_FILE_INFO_MTIME);
    std::snprintf(info->file.type, sizeof(info->file.type), "%s", "image/jpeg");
    info->file.size = std::uint64_t{1} << 40;
    info->file.width = 6000;
    info->file.height = 4000;
    info->file.permissions = static_cast<CameraFilePermissions>(
        GP_FILE_PERM_READ | GP_FILE_PERM_DELETE);
    info->file.status = GP_FILE_STATUS_DOWNLOADED;
    info->file.mtime = 1600000000;

    info->preview.fields = static_cast<CameraFileInfoFields>(
        GP_FILE_INFO_TYPE | GP_FILE_INFO_SIZE | GP_FILE_INFO_STATUS |
        GP_FILE_INFO_WIDTH | GP_FILE_INFO_HEIGHT);
    std::snprintf(info->preview.type, sizeof(info->preview.type), "%s", "image/png");
    info->preview.size = 123;
    info->preview.status = GP_FILE_STATUS_NOT_DOWNLOADED;
    info->preview.width = 640;
    info->preview.height = 480;

    info->audio.fields = static_cast<CameraFileInfoFields>(
        GP_FILE_INFO_TYPE | GP_FILE_INFO_SIZE | GP_FILE_INFO_STATUS);
    std::snprintf(info->audio.type, sizeof(info->audio.type), "%s", "audio/wav");
    info->audio.size = 42;
    info->audio.status = GP_FILE_STATUS_DOWNLOADED;

    if (next_file_info_result == file_info_result::partial) {
        info->preview.fields = GP_FILE_INFO_WIDTH;
        std::memset(info->preview.type, 'x', sizeof(info->preview.type));
        info->audio.fields = GP_FILE_INFO_TYPE;
        info->audio.type[0] = '\0';
    } else if (next_file_info_result == file_info_result::bad_file_type) {
        std::memset(info->file.type, 'x', sizeof(info->file.type));
    } else if (next_file_info_result == file_info_result::bad_preview_type) {
        std::memset(info->preview.type, 'x', sizeof(info->preview.type));
    } else if (next_file_info_result == file_info_result::bad_audio_type) {
        std::memset(info->audio.type, 'x', sizeof(info->audio.type));
    }
    return GP_OK;
}

extern "C" int gp_camera_file_set_info(
    Camera *, const char *folder, const char *file, CameraFileInfo info,
    GPContext *) {
    requested_folder = folder;
    requested_file = file;
    last_file_update = info;
    ++file_update_calls;
    return file_update_result;
}

}  // namespace gphoto2

class CameraWrapper_FileInfo : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;
    std::unique_ptr<gphoto2pp::CameraWrapper> m_camera;

public:
    void setUp() {
        next_file_info_result = file_info_result::populated;
        requested_folder.clear();
        requested_file.clear();
        file_info_calls = 0;
        file_update_calls = 0;
        file_update_result = GP_OK;
        if (gphoto2::gp_port_info_list_new(&m_ports) != GP_OK ||
            gphoto2::gp_port_info_new(&fake_port) != GP_OK ||
            gphoto2::gp_port_info_set_name(fake_port, "Synthetic metadata port") != GP_OK ||
            gphoto2::gp_port_info_set_type(fake_port, gphoto2::GP_PORT_USB) != GP_OK ||
            gphoto2::gp_port_info_set_path(fake_port, "fake:no-usb") != GP_OK ||
            gphoto2::gp_port_info_list_append(m_ports, fake_port) < GP_OK) {
            throw std::runtime_error("Unable to configure synthetic metadata port");
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

    void testFileMetadataPreservesIndependentFields() {
        const auto info = m_camera->fileGetInfo("/DCIM", "image.jpg");
        TS_ASSERT_EQUALS(requested_folder, "/DCIM");
        TS_ASSERT_EQUALS(requested_file, "image.jpg");
        TS_ASSERT_EQUALS(file_info_calls, 1);

        TS_ASSERT(info.file.common.has(gphoto2pp::CameraFileInfoField::Type));
        TS_ASSERT_EQUALS(info.file.common.mimeType, "image/jpeg");
        TS_ASSERT_EQUALS(info.file.common.size, std::uint64_t{1} << 40);
        TS_ASSERT_EQUALS(info.file.width, 6000);
        TS_ASSERT_EQUALS(info.file.height, 4000);
        TS_ASSERT_EQUALS(info.file.modificationTime, 1600000000);
        TS_ASSERT(info.file.common.has(
            gphoto2pp::CameraFileInfoField::Permissions));
        TS_ASSERT_EQUALS(info.file.permissions,
            static_cast<std::uint32_t>(gphoto2pp::CameraFilePermission::Read) |
            static_cast<std::uint32_t>(gphoto2pp::CameraFilePermission::Delete));
        TS_ASSERT_EQUALS(info.file.common.status,
            gphoto2pp::CameraFileStatus::Downloaded);

        TS_ASSERT_EQUALS(info.preview.common.mimeType, "image/png");
        TS_ASSERT_EQUALS(info.preview.width, 640);
        TS_ASSERT_EQUALS(info.preview.height, 480);
        TS_ASSERT_EQUALS(info.audio.mimeType, "audio/wav");
        TS_ASSERT_EQUALS(info.audio.size, 42);
        TS_ASSERT_EQUALS(info.audio.status,
            gphoto2pp::CameraFileStatus::Downloaded);
    }

    void testPartialMasksAndMalformedMimeTypes() {
        next_file_info_result = file_info_result::partial;
        const auto partial = m_camera->fileGetInfo("/DCIM", "image.jpg");
        TS_ASSERT(partial.preview.common.has(
            gphoto2pp::CameraFileInfoField::Width));
        TS_ASSERT(!partial.preview.common.has(
            gphoto2pp::CameraFileInfoField::Type));
        TS_ASSERT_EQUALS(partial.preview.width, 640);
        TS_ASSERT(partial.audio.has(gphoto2pp::CameraFileInfoField::Type));
        TS_ASSERT(partial.audio.mimeType.empty());
        TS_ASSERT(!partial.audio.has(gphoto2pp::CameraFileInfoField::Size));

        const file_info_result malformed[] = {
            file_info_result::bad_file_type,
            file_info_result::bad_preview_type,
            file_info_result::bad_audio_type
        };
        for (const auto scenario : malformed) {
            next_file_info_result = scenario;
            bool rejected = false;
            try {
                m_camera->fileGetInfo("/DCIM", "image.jpg");
            } catch (const std::runtime_error &) {
                rejected = true;
            }
            TS_ASSERT(rejected);
        }
    }

    void testFileMetadataErrorsPreserveLibgphotoCodes() {
        const int errors[] = {
            GP_ERROR_NOT_SUPPORTED,
            GP_ERROR_IO
        };
        for (const int result : errors) {
            next_file_info_result = result == GP_ERROR_NOT_SUPPORTED ?
                file_info_result::unsupported : file_info_result::io_error;
            bool preserved = false;
            try {
                m_camera->fileGetInfo("/DCIM", "image.jpg");
            } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
                preserved = error.getResultCode() == result;
            }
            TS_ASSERT(preserved);
        }
    }

    void testMetadataPathsRejectEmbeddedNulBeforeCameraIo() {
        const std::string nul_folder("/DCIM\0other", 11);
        const std::string nul_file("image\0.jpg", 10);
        bool rejected = false;
        try {
            m_camera->fileGetInfo(nul_folder, "image.jpg");
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_info_calls, 0);

        rejected = false;
        try {
            m_camera->fileGetInfo("/DCIM", nul_file);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_info_calls, 0);

        gphoto2pp::CameraFileInfoUpdate update;
        update.updatePermissions = true;
        update.permissions =
            static_cast<std::uint32_t>(gphoto2pp::CameraFilePermission::Read);
        rejected = false;
        try {
            m_camera->fileSetInfo(nul_folder, "image.jpg", update);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_update_calls, 0);

        rejected = false;
        try {
            m_camera->fileSetInfo("/DCIM", nul_file, update);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_update_calls, 0);
    }

    void testMetadataUpdatesValidateAndPreserveSelectedFields() {
        gphoto2pp::CameraFileInfoUpdate update;
        bool rejected = false;
        try {
            m_camera->fileSetInfo("/DCIM", "image.jpg", update);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_update_calls, 0);

        update.updatePermissions = true;
        update.permissions =
            static_cast<std::uint32_t>(gphoto2pp::CameraFilePermission::Read);
        m_camera->fileSetInfo("/DCIM", "image.jpg", update);
        TS_ASSERT_EQUALS(file_update_calls, 1);
        TS_ASSERT_EQUALS(requested_folder, "/DCIM");
        TS_ASSERT_EQUALS(requested_file, "image.jpg");
        TS_ASSERT_EQUALS(last_file_update.file.fields,
            gphoto2::GP_FILE_INFO_PERMISSIONS);
        TS_ASSERT_EQUALS(last_file_update.file.permissions,
            gphoto2::GP_FILE_PERM_READ);
        TS_ASSERT_EQUALS(last_file_update.preview.fields,
            gphoto2::GP_FILE_INFO_NONE);
        TS_ASSERT_EQUALS(last_file_update.audio.fields, gphoto2::GP_FILE_INFO_NONE);

        update.updateModificationTime = true;
        update.permissions = 0;
        update.modificationTime = 0;
        m_camera->fileSetInfo("/DCIM", "image.jpg", update);
        TS_ASSERT_EQUALS(last_file_update.file.fields,
            gphoto2::GP_FILE_INFO_PERMISSIONS | gphoto2::GP_FILE_INFO_MTIME);
        TS_ASSERT_EQUALS(last_file_update.file.permissions, gphoto2::GP_FILE_PERM_NONE);
        TS_ASSERT_EQUALS(last_file_update.file.mtime, 0);

        update.updatePermissions = false;
        update.modificationTime = 1600000000;
        m_camera->fileSetInfo("/DCIM", "image.jpg", update);
        TS_ASSERT_EQUALS(last_file_update.file.fields, gphoto2::GP_FILE_INFO_MTIME);
        TS_ASSERT_EQUALS(last_file_update.file.mtime, 1600000000);

        update.updatePermissions = true;
        update.permissions =
            static_cast<std::uint32_t>(gphoto2pp::CameraFilePermission::Read) |
            static_cast<std::uint32_t>(gphoto2pp::CameraFilePermission::Delete);
        m_camera->fileSetInfo("/DCIM", "image.jpg", update);
        TS_ASSERT_EQUALS(last_file_update.file.permissions,
            gphoto2::GP_FILE_PERM_READ | gphoto2::GP_FILE_PERM_DELETE);

        update.permissions = 4;
        rejected = false;
        try {
            m_camera->fileSetInfo("/DCIM", "image.jpg", update);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(file_update_calls, 4);
    }

    void testMetadataUpdateErrorsPreserveLibgphotoCodes() {
        gphoto2pp::CameraFileInfoUpdate update;
        update.updateModificationTime = true;
        update.modificationTime = 1600000000;
        const int errors[] = {
            GP_ERROR_NOT_SUPPORTED,
            GP_ERROR_IO
        };
        for (const int result : errors) {
            file_update_result = result;
            bool preserved = false;
            try {
                m_camera->fileSetInfo("/DCIM", "image.jpg", update);
            } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
                preserved = error.getResultCode() == result;
            }
            TS_ASSERT(preserved);
        }
    }
};
