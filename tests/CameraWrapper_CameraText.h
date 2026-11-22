#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-abilities-list.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <utility>

namespace {

enum class text_result { valid, unterminated, unsupported };

text_result next_text_result = text_result::valid;
gphoto2::GPPortInfo fake_port = nullptr;
int exit_calls = 0;
int exit_result = GP_OK;
int init_calls = 0;
int set_abilities_calls = 0;
int set_abilities_result = GP_OK;
gphoto2::CameraAbilities last_abilities{};
int set_port_info_calls = 0;
int set_port_info_result = GP_OK;

int write_text(gphoto2::CameraText *text, const char *value) {
    if (next_text_result == text_result::unsupported) {
        return GP_ERROR_NOT_SUPPORTED;
    }
    if (next_text_result == text_result::unterminated) {
        std::memset(text->text, 'x', sizeof(text->text));
        return GP_OK;
    }
    std::snprintf(text->text, sizeof(text->text), "%s", value);
    return GP_OK;
}

}  // namespace

namespace gphoto2 {

extern "C" int gp_camera_init(Camera *, GPContext *) {
    ++init_calls;
    return GP_OK;
}

extern "C" int gp_camera_exit(Camera *, GPContext *) {
    ++exit_calls;
    return exit_result;
}

extern "C" int gp_camera_get_port_info(Camera *, GPPortInfo *info) {
    *info = fake_port;
    return GP_OK;
}

extern "C" int gp_camera_get_abilities(Camera *, CameraAbilities *abilities) {
    std::memset(abilities, 0, sizeof(*abilities));
    std::snprintf(abilities->model, sizeof(abilities->model), "%s", "Synthetic camera");
    abilities->status = gphoto2::GP_DRIVER_STATUS_TESTING;
    abilities->port = gphoto2::GP_PORT_USB;
    abilities->speed[0] = 9600;
    abilities->speed[1] = 115200;
    abilities->operations = gphoto2::GP_OPERATION_CAPTURE_IMAGE;
    abilities->file_operations = gphoto2::GP_FILE_OPERATION_DELETE;
    abilities->folder_operations = gphoto2::GP_FOLDER_OPERATION_MAKE_DIR;
    abilities->usb_vendor = 0x1234;
    abilities->usb_product = 0x5678;
    abilities->device_type = gphoto2::GP_DEVICE_STILL_CAMERA;
    std::snprintf(abilities->library, sizeof(abilities->library),
        "%s", "synthetic.so");
    std::snprintf(abilities->id, sizeof(abilities->id),
        "%s", "Synthetic ID");
    return GP_OK;
}

extern "C" int gp_camera_set_abilities(
    Camera *, CameraAbilities abilities) {
    ++set_abilities_calls;
    last_abilities = abilities;
    return set_abilities_result;
}

extern "C" int gp_camera_set_port_info(
    Camera *, GPPortInfo info) {
    ++set_port_info_calls;
    return set_port_info_result;
}

extern "C" int gp_camera_get_summary(Camera *, CameraText *text, GPContext *) {
    return write_text(text, "summary");
}

extern "C" int gp_camera_get_manual(Camera *, CameraText *text, GPContext *) {
    return write_text(text, "manual");
}

extern "C" int gp_camera_get_about(Camera *, CameraText *text, GPContext *) {
    return write_text(text, "about");
}

}  // namespace gphoto2

class CameraWrapper_CameraText : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;
    std::unique_ptr<gphoto2pp::CameraWrapper> m_camera;

public:
    void setUp() {
        next_text_result = text_result::valid;
        exit_calls = 0;
        exit_result = GP_OK;
        init_calls = 0;
        set_abilities_calls = 0;
        set_abilities_result = GP_OK;
        last_abilities = {};
        set_port_info_calls = 0;
        set_port_info_result = GP_OK;
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

    void testCameraPortInfoRoundTrip() {
        const auto initial = m_camera->getPortInfo();
        TS_ASSERT_EQUALS(initial.name, "Synthetic port");
        TS_ASSERT_EQUALS(initial.path, "fake:no-usb");
        TS_ASSERT_EQUALS(initial.type, gphoto2pp::GPPortTypeWrapper::USB);

        gphoto2pp::PortInfoSnapshot updated;
        updated.name = "Updated port";
        updated.path = "usb:002";
        updated.type = gphoto2pp::GPPortTypeWrapper::USB;
        m_camera->setPortInfo(updated);
        TS_ASSERT_EQUALS(set_port_info_calls, 1);

        set_port_info_result = GP_ERROR_IO;
        bool preserved_error = false;
        try {
            m_camera->setPortInfo(updated);
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(preserved_error);

        updated.name = std::string("bad\0name", 8);
        TS_ASSERT_THROWS(m_camera->setPortInfo(updated),
            const gphoto2pp::exceptions::ArgumentException &);
    }

    void testCameraInformationText() {
        next_text_result = text_result::valid;
        TS_ASSERT_EQUALS(m_camera->getSummary(), "summary");
        TS_ASSERT_EQUALS(m_camera->getManual(), "manual");
        TS_ASSERT_EQUALS(m_camera->getAbout(), "about");

        next_text_result = text_result::unsupported;
        bool propagated = false;
        try {
            m_camera->getManual();
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            propagated = error.isNotSupported();
        }
        TS_ASSERT(propagated);

        next_text_result = text_result::unterminated;
        bool rejected = false;
        try {
            m_camera->getAbout();
        } catch (const std::runtime_error &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
    }

    void testCameraAbilitiesRoundTrip() {
        const auto snapshot = m_camera->getAbilities();
        TS_ASSERT_EQUALS(snapshot.model, "Synthetic camera");
        TS_ASSERT_EQUALS(snapshot.status,
            gphoto2pp::CameraDriverStatus::Testing);
        TS_ASSERT_EQUALS(snapshot.port,
            static_cast<std::uint32_t>(gphoto2::GP_PORT_USB));
        TS_ASSERT_EQUALS(snapshot.speeds.size(), 2U);
        TS_ASSERT_EQUALS(snapshot.speeds[0], 9600);
        TS_ASSERT_EQUALS(snapshot.speeds[1], 115200);
        TS_ASSERT_EQUALS(snapshot.usbVendor, 0x1234);
        TS_ASSERT_EQUALS(snapshot.usbProduct, 0x5678);
        TS_ASSERT_EQUALS(snapshot.library, "synthetic.so");
        TS_ASSERT_EQUALS(snapshot.id, "Synthetic ID");

        m_camera->setAbilities(snapshot);
        TS_ASSERT_EQUALS(set_abilities_calls, 1);
        TS_ASSERT_EQUALS(std::string(last_abilities.model),
            "Synthetic camera");

        set_abilities_result = GP_ERROR_IO;
        bool preserved_error = false;
        try {
            m_camera->setAbilities(snapshot);
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(preserved_error);
    }

    void testExplicitExitAndMovedFromCamera() {
        m_camera->exit();
        TS_ASSERT_EQUALS(exit_calls, 1);
        m_camera->reinitialize();
        TS_ASSERT(m_camera->getContext().getPtr() != nullptr);
        m_camera->exit();
        TS_ASSERT_EQUALS(exit_calls, 2);
        bool rejected = false;
        try {
            m_camera->exit();
        } catch (const gphoto2pp::exceptions::CameraWrapperException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);

        exit_result = GP_ERROR_IO;
        gphoto2pp::CameraWrapper new_camera;
        bool error_preserved = false;
        try {
            new_camera.exit();
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            error_preserved = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(error_preserved);

        gphoto2pp::CameraWrapper moved(std::move(new_camera));
        bool moved_rejected = false;
        try {
            new_camera.exit();
        } catch (const gphoto2pp::exceptions::CameraWrapperException &) {
            moved_rejected = true;
        }
        TS_ASSERT(moved_rejected);
    }
};
