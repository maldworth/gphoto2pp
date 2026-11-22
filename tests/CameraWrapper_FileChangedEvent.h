#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_event_type_wrapper.hpp>
#include <gphoto2pp/camera_file_path_wrapper.hpp>
#include <gphoto2pp/camera_wrapper.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

namespace {

gphoto2::GPPortInfo fake_port = nullptr;
std::mutex event_mutex;
std::condition_variable event_condition;
bool file_changed_event_pending = false;
bool file_changed_event_received = false;
std::string received_file_name;
std::string received_folder;
std::string received_event_data;

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
    std::snprintf(
        abilities->model, sizeof(abilities->model), "%s", "Synthetic camera");
    return GP_OK;
}

extern "C" int gp_camera_wait_for_event(
    Camera *, int, CameraEventType *type, void **data, GPContext *) {
    std::unique_lock<std::mutex> lock(event_mutex);
    event_condition.wait_for(lock, std::chrono::milliseconds(20), [] {
        return file_changed_event_pending;
    });

    if (file_changed_event_pending) {
        static CameraFilePath file_path{"changed.jpg", "/DCIM"};
        file_changed_event_pending = false;
        *type = GP_EVENT_FILE_CHANGED;
        *data = &file_path;
    } else {
        *type = GP_EVENT_TIMEOUT;
        *data = nullptr;
    }
    return GP_OK;
}

}  // namespace gphoto2

class CameraWrapper_FileChangedEvent : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;
    std::unique_ptr<gphoto2pp::CameraWrapper> m_camera;

public:
    void setUp() {
        {
            std::lock_guard<std::mutex> lock(event_mutex);
            file_changed_event_pending = false;
            file_changed_event_received = false;
            received_file_name.clear();
            received_folder.clear();
            received_event_data.clear();
        }

        if (gphoto2::gp_port_info_list_new(&m_ports) != GP_OK ||
            gphoto2::gp_port_info_new(&fake_port) != GP_OK ||
            gphoto2::gp_port_info_set_name(fake_port, "Synthetic event port") != GP_OK ||
            gphoto2::gp_port_info_set_type(fake_port, gphoto2::GP_PORT_USB) != GP_OK ||
            gphoto2::gp_port_info_set_path(fake_port, "fake:no-usb") != GP_OK ||
            gphoto2::gp_port_info_list_append(m_ports, fake_port) < GP_OK) {
            throw std::runtime_error("Unable to configure synthetic event port");
        }
        m_camera.reset(new gphoto2pp::CameraWrapper());
    }

    void tearDown() {
        if (m_camera != nullptr) {
            m_camera->stopListeningForEvents();
            m_camera.reset();
        }
        if (m_ports != nullptr) {
            gphoto2::gp_port_info_list_free(m_ports);
            m_ports = nullptr;
        }
        fake_port = nullptr;
    }

    void testFileChangedEventDeliversOwnedPath() {
        auto registration = m_camera->subscribeToCameraEvent(
            gphoto2pp::CameraEventTypeWrapper::FileChanged,
            [](const gphoto2pp::CameraFilePathWrapper &path,
                const std::string &data) {
                std::lock_guard<std::mutex> lock(event_mutex);
                received_file_name = path.Name;
                received_folder = path.Folder;
                received_event_data = data;
                file_changed_event_received = true;
                event_condition.notify_all();
            });

        TS_ASSERT(m_camera->startListeningForEvents());
        {
            std::lock_guard<std::mutex> lock(event_mutex);
            file_changed_event_pending = true;
        }
        event_condition.notify_all();

        bool received = false;
        {
            std::unique_lock<std::mutex> lock(event_mutex);
            received = event_condition.wait_for(
                lock, std::chrono::seconds(1), [] {
                    return file_changed_event_received;
                });
        }
        m_camera->stopListeningForEvents();

        TS_ASSERT(received);
        if (received) {
            TS_ASSERT_EQUALS(received_file_name, "changed.jpg");
            TS_ASSERT_EQUALS(received_folder, "/DCIM");
            TS_ASSERT(received_event_data.empty());
        }
    }
};
