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
namespace {
int port_speed = 9600;
int get_speed_result = GP_OK;
int set_speed_result = GP_OK;
int set_speed_calls = 0;
int requested_speed = 0;
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

extern "C" int gp_camera_get_port_speed(Camera *) {
    return get_speed_result == GP_OK ? port_speed : get_speed_result;
}

extern "C" int gp_camera_set_port_speed(Camera *, int speed) {
    ++set_speed_calls;
    requested_speed = speed;
    if (set_speed_result == GP_OK) {
        port_speed = speed;
    }
    return set_speed_result;
}

}  // namespace gphoto2

class CameraWrapper_PortSpeed : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;
    std::unique_ptr<gphoto2pp::CameraWrapper> m_camera;

public:
    void setUp() {
        port_speed = 9600;
        get_speed_result = GP_OK;
        set_speed_result = GP_OK;
        set_speed_calls = 0;
        requested_speed = 0;
        if (gphoto2::gp_port_info_list_new(&m_ports) != GP_OK ||
            gphoto2::gp_port_info_new(&fake_port) != GP_OK ||
            gphoto2::gp_port_info_set_name(fake_port, "Synthetic serial port") != GP_OK ||
            gphoto2::gp_port_info_set_type(fake_port, gphoto2::GP_PORT_SERIAL) != GP_OK ||
            gphoto2::gp_port_info_set_path(fake_port, "serial:/dev/fake") != GP_OK ||
            gphoto2::gp_port_info_list_append(m_ports, fake_port) < GP_OK) {
            throw std::runtime_error("Unable to configure synthetic serial port");
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

    void testPortSpeedRoundTrip() {
        TS_ASSERT_EQUALS(m_camera->getPortSpeed(), 9600);
        port_speed = 0;
        TS_ASSERT_EQUALS(m_camera->getPortSpeed(), 0);
        m_camera->setPortSpeed(115200);
        TS_ASSERT_EQUALS(set_speed_calls, 1);
        TS_ASSERT_EQUALS(requested_speed, 115200);
        TS_ASSERT_EQUALS(m_camera->getPortSpeed(), 115200);
    }

    void testPortSpeedErrorsPropagate() {
        get_speed_result = GP_ERROR_NOT_SUPPORTED;
        bool get_error_preserved = false;
        try {
            m_camera->getPortSpeed();
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            get_error_preserved = error.isNotSupported();
        }
        TS_ASSERT(get_error_preserved);

        set_speed_result = GP_ERROR_NOT_SUPPORTED;
        bool set_error_preserved = false;
        try {
            m_camera->setPortSpeed(19200);
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            set_error_preserved = error.isNotSupported();
        }
        TS_ASSERT(set_error_preserved);
    }
};
