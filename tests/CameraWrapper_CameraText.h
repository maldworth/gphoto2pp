#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>

namespace {

enum class text_result { valid, unterminated, unsupported };

text_result next_text_result = text_result::valid;
gphoto2::GPPortInfo fake_port = nullptr;

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
};
