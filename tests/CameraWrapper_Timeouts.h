#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <cstdio>
#include <climits>
#include <cstdint>
#include <cstring>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace {

gphoto2::GPPortInfo fake_port = nullptr;
gphoto2::CameraTimeoutStartFunc timeout_start = nullptr;
gphoto2::CameraTimeoutStopFunc timeout_stop = nullptr;
void *timeout_data = nullptr;
gphoto2::Camera *active_camera = nullptr;
unsigned int active_timeout_id = 0;
bool request_timeout_during_init = false;
int timeout_seconds = 0;
gphoto2::GPContext *timeout_context = nullptr;
gphoto2pp::CameraTimeoutAction scheduled_action;
std::string timeout_events;
int camera_new_calls = 0;

int keep_alive(gphoto2::Camera *camera, gphoto2::GPContext *context) {
    active_camera = camera;
    timeout_context = context;
    return GP_OK;
}

}  // namespace

namespace gphoto2 {

extern "C" int gp_camera_new(Camera **camera) {
    ++camera_new_calls;
    *camera = reinterpret_cast<Camera *>(static_cast<std::uintptr_t>(1));
    return GP_OK;
}

extern "C" int gp_camera_unref(Camera *) {
    return GP_OK;
}

extern "C" int gp_camera_init(Camera *camera, GPContext *) {
    timeout_events += 'I';
    if (request_timeout_during_init && timeout_start != nullptr) {
        active_timeout_id = timeout_start(
            camera, 4, keep_alive, timeout_data);
        if (active_timeout_id > static_cast<unsigned int>(INT_MAX)) {
            return static_cast<int>(active_timeout_id);
        }
    }
    return GP_OK;
}

extern "C" int gp_camera_exit(Camera *camera, GPContext *) {
    if (timeout_stop != nullptr && active_timeout_id != 0) {
        timeout_stop(camera, active_timeout_id, timeout_data);
        active_timeout_id = 0;
    }
    return GP_OK;
}

extern "C" void gp_camera_set_timeout_funcs(
    Camera *, CameraTimeoutStartFunc start, CameraTimeoutStopFunc stop,
    void *data) {
    timeout_start = start;
    timeout_stop = stop;
    timeout_data = data;
    timeout_events += start == nullptr ? 'C' : 'S';
}

extern "C" int gp_camera_get_port_info(Camera *, GPPortInfo *info) {
    *info = fake_port;
    return GP_OK;
}

extern "C" int gp_camera_get_abilities(
    Camera *, CameraAbilities *abilities) {
    std::memset(abilities, 0, sizeof(*abilities));
    std::snprintf(abilities->model, sizeof(abilities->model), "%s",
        "Synthetic timeout camera");
    abilities->port = GP_PORT_USB;
    return GP_OK;
}

}  // namespace gphoto2

class CameraWrapper_Timeouts : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;

public:
    void setUp() {
        timeout_start = nullptr;
        timeout_stop = nullptr;
        timeout_data = nullptr;
        active_camera = nullptr;
        active_timeout_id = 0;
        request_timeout_during_init = false;
        timeout_seconds = 0;
        timeout_context = nullptr;
        scheduled_action = {};
        timeout_events.clear();
        camera_new_calls = 0;
        if (gphoto2::gp_port_info_list_new(&m_ports) != GP_OK ||
            gphoto2::gp_port_info_new(&fake_port) != GP_OK ||
            gphoto2::gp_port_info_set_name(fake_port, "Synthetic port") != GP_OK ||
            gphoto2::gp_port_info_set_type(fake_port, gphoto2::GP_PORT_USB) != GP_OK ||
            gphoto2::gp_port_info_set_path(fake_port, "fake:timeout") != GP_OK ||
            gphoto2::gp_port_info_list_append(m_ports, fake_port) < GP_OK) {
            throw std::runtime_error(
                "Unable to configure synthetic timeout port");
        }
    }

    void tearDown() {
        scheduled_action = {};
        if (m_ports != nullptr) {
            gphoto2::gp_port_info_list_free(m_ports);
            m_ports = nullptr;
        }
        fake_port = nullptr;
    }

    void testConstructorRegistersBeforeCameraInitialization() {
        request_timeout_during_init = true;
        gphoto2pp::CameraTimeoutCallbacks callbacks;
        callbacks.start = [](unsigned int seconds,
            gphoto2pp::CameraTimeoutAction action) {
            timeout_seconds = static_cast<int>(seconds);
            scheduled_action = std::move(action);
            return 41U;
        };
        callbacks.stop = [](unsigned int id) {
            TS_ASSERT_EQUALS(id, 41U);
        };

        {
            gphoto2pp::CameraWrapper camera(callbacks);
            TS_ASSERT_EQUALS(timeout_events.substr(0, 2), "SI");
            TS_ASSERT_EQUALS(timeout_seconds, 4);
            TS_ASSERT_DIFFERS(scheduled_action, nullptr);
            TS_ASSERT_EQUALS(scheduled_action(), GP_OK);
            TS_ASSERT_EQUALS(active_camera,
                reinterpret_cast<gphoto2::Camera *>(
                    static_cast<std::uintptr_t>(1)));
            TS_ASSERT_DIFFERS(timeout_context, nullptr);

            camera.exit();
            TS_ASSERT_EQUALS(scheduled_action(), GP_ERROR_CANCEL);
        }

        TS_ASSERT_EQUALS(timeout_events.substr(
            timeout_events.size() - 1), "C");
    }

    void testCallbacksCanBeSetAndRemovedWhenNoTimeoutIsActive() {
        gphoto2pp::CameraWrapper camera;
        gphoto2pp::CameraTimeoutCallbacks callbacks;
        callbacks.start = [](unsigned int,
            gphoto2pp::CameraTimeoutAction) { return 7U; };
        callbacks.stop = [](unsigned int) {};

        camera.setTimeoutCallbacks(callbacks);
        TS_ASSERT_DIFFERS(timeout_start, nullptr);
        TS_ASSERT_DIFFERS(timeout_stop, nullptr);

        gphoto2::CameraTimeoutFunc function = keep_alive;
        active_timeout_id = timeout_start(
            reinterpret_cast<gphoto2::Camera *>(
                static_cast<std::uintptr_t>(1)),
            1, function, timeout_data);
        TS_ASSERT_EQUALS(active_timeout_id, 7U);
        TS_ASSERT_THROWS(camera.setTimeoutCallbacks({}),
            const gphoto2pp::exceptions::CameraWrapperException &);

        timeout_stop(
            reinterpret_cast<gphoto2::Camera *>(
                static_cast<std::uintptr_t>(1)),
            active_timeout_id, timeout_data);
        active_timeout_id = 0;
        camera.setTimeoutCallbacks({});
        TS_ASSERT_EQUALS(timeout_start, nullptr);
        TS_ASSERT_EQUALS(timeout_stop, nullptr);
    }

    void testTimeoutCallbacksMustBeConfiguredAsAPair() {
        gphoto2pp::CameraTimeoutCallbacks callbacks;
        callbacks.start = [](unsigned int,
            gphoto2pp::CameraTimeoutAction) { return 1U; };
        TS_ASSERT_THROWS(gphoto2pp::CameraWrapper camera(callbacks),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_EQUALS(camera_new_calls, 0);
    }

    void testCallbacksCannotBeReplacedWhileStartIsInFlight() {
        gphoto2pp::CameraWrapper camera;
        std::promise<void> start_entered;
        std::promise<void> continue_start;
        auto start_released = continue_start.get_future();
        gphoto2pp::CameraTimeoutCallbacks callbacks;
        callbacks.start = [&](unsigned int,
            gphoto2pp::CameraTimeoutAction action) {
            scheduled_action = std::move(action);
            start_entered.set_value();
            start_released.wait();
            return 55U;
        };
        callbacks.stop = [](unsigned int) {};
        camera.setTimeoutCallbacks(callbacks);

        std::thread starter([&]() {
            active_timeout_id = timeout_start(
                reinterpret_cast<gphoto2::Camera *>(
                    static_cast<std::uintptr_t>(1)),
                1, keep_alive, timeout_data);
        });
        start_entered.get_future().wait();
        TS_ASSERT_THROWS(camera.setTimeoutCallbacks({}),
            const gphoto2pp::exceptions::CameraWrapperException &);
        continue_start.set_value();
        starter.join();

        TS_ASSERT_EQUALS(active_timeout_id, 55U);
        timeout_stop(
            reinterpret_cast<gphoto2::Camera *>(
                static_cast<std::uintptr_t>(1)),
            active_timeout_id, timeout_data);
        active_timeout_id = 0;
        camera.setTimeoutCallbacks({});
    }
};
