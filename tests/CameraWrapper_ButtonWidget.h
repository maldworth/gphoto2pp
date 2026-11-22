#include <cxxtest/TestSuite.h>

#include <gphoto2pp/button_widget.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/window_widget.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-port-info-list.h>
#include <gphoto2/gphoto2-widget.h>
}

#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

enum class config_layout {
    single_button,
    duplicate_button,
    non_button,
    missing_button,
    button_without_callback
};

enum class config_fetch_result {
    success,
    io_error,
    null_config
};

config_layout current_layout = config_layout::single_button;
config_fetch_result current_config_fetch_result =
    config_fetch_result::success;
gphoto2::GPPortInfo fake_port = nullptr;
int config_calls = 0;
int callback_calls = 0;
int callback_result = GP_OK;
gphoto2::Camera *config_camera = nullptr;
gphoto2::GPContext *config_context = nullptr;
gphoto2::Camera *callback_camera = nullptr;
gphoto2::GPContext *callback_context = nullptr;
std::string callback_widget_name;

int synthetic_button_callback(
    gphoto2::Camera *camera, gphoto2::CameraWidget *widget,
    gphoto2::GPContext *context) {
    ++callback_calls;
    callback_camera = camera;
    callback_context = context;
    const char *name = nullptr;
    if (gphoto2::gp_widget_get_name(widget, &name) == GP_OK && name != nullptr) {
        callback_widget_name = name;
    }
    return callback_result;
}

int append_widget(gphoto2::CameraWidget *root,
    gphoto2::CameraWidgetType type, const char *name, bool with_callback) {
    gphoto2::CameraWidget *widget = nullptr;
    int result = gphoto2::gp_widget_new(type, name, &widget);
    if (result < GP_OK) {
        return result;
    }
    result = gphoto2::gp_widget_set_name(widget, name);
    if (result == GP_OK && type == gphoto2::GP_WIDGET_BUTTON && with_callback) {
        result = gphoto2::gp_widget_set_value(widget,
            reinterpret_cast<const void *>(synthetic_button_callback));
    } else if (result == GP_OK && type == gphoto2::GP_WIDGET_TEXT) {
        result = gphoto2::gp_widget_set_value(widget, "text");
    }
    if (result == GP_OK) {
        result = gphoto2::gp_widget_append(root, widget);
    }
    if (result < GP_OK) {
        gphoto2::gp_widget_unref(widget);
    }
    return result;
}

int create_config(gphoto2::CameraWidget **root) {
    int result = gphoto2::gp_widget_new(
        gphoto2::GP_WIDGET_WINDOW, "Camera", root);
    if (result < GP_OK) {
        return result;
    }
    result = gphoto2::gp_widget_set_name(*root, "root");
    if (result == GP_OK) {
        switch (current_layout) {
            case config_layout::single_button:
                result = append_widget(
                    *root, gphoto2::GP_WIDGET_BUTTON, "execute", true);
                break;
            case config_layout::duplicate_button:
                result = append_widget(
                    *root, gphoto2::GP_WIDGET_BUTTON, "execute", true);
                if (result == GP_OK) {
                    result = append_widget(
                        *root, gphoto2::GP_WIDGET_BUTTON, "execute", true);
                }
                break;
            case config_layout::non_button:
                result = append_widget(
                    *root, gphoto2::GP_WIDGET_TEXT, "execute", false);
                break;
            case config_layout::missing_button:
                result = append_widget(
                    *root, gphoto2::GP_WIDGET_BUTTON, "other", true);
                break;
            case config_layout::button_without_callback:
                result = append_widget(
                    *root, gphoto2::GP_WIDGET_BUTTON, "execute", false);
                break;
        }
    }
    if (result < GP_OK) {
        gphoto2::gp_widget_unref(*root);
        *root = nullptr;
    }
    return result;
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

extern "C" int gp_camera_get_config(
    Camera *camera, CameraWidget **window, GPContext *context) {
    ++config_calls;
    config_camera = camera;
    config_context = context;
    if (current_config_fetch_result == config_fetch_result::io_error) {
        return GP_ERROR_IO;
    }
    if (current_config_fetch_result == config_fetch_result::null_config) {
        *window = nullptr;
        return GP_OK;
    }
    return create_config(window);
}

}  // namespace gphoto2

class CameraWrapper_ButtonWidget : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;
    std::unique_ptr<gphoto2pp::CameraWrapper> m_camera;

public:
    void setUp() {
        current_layout = config_layout::single_button;
        current_config_fetch_result = config_fetch_result::success;
        config_calls = 0;
        callback_calls = 0;
        callback_result = GP_OK;
        config_camera = nullptr;
        config_context = nullptr;
        callback_camera = nullptr;
        callback_context = nullptr;
        callback_widget_name.clear();
        if (gphoto2::gp_port_info_list_new(&m_ports) != GP_OK ||
            gphoto2::gp_port_info_new(&fake_port) != GP_OK ||
            gphoto2::gp_port_info_set_name(fake_port, "Synthetic button port") != GP_OK ||
            gphoto2::gp_port_info_set_type(fake_port, gphoto2::GP_PORT_USB) != GP_OK ||
            gphoto2::gp_port_info_set_path(fake_port, "fake:no-usb") != GP_OK ||
            gphoto2::gp_port_info_list_append(m_ports, fake_port) < GP_OK) {
            throw std::runtime_error("Unable to configure synthetic button port");
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

    void testConfigUsesTypedButtonWidget() {
        auto config = m_camera->getConfig();
        auto button = config.getChild<gphoto2pp::ButtonWidget>(0);
        TS_ASSERT_EQUALS(button.getName(), "execute");
        TS_ASSERT_EQUALS(button.getType(),
            gphoto2pp::CameraWidgetTypeWrapper::Button);
        TS_ASSERT(button.hasCallback());
        button.setCallback(reinterpret_cast<const void *>(
            synthetic_button_callback));
        TS_ASSERT(button.hasCallback());
    }

    void testCameraWrapperExposesContext() {
        TS_ASSERT(m_camera->getContext().getPtr() != nullptr);
        const auto &context = m_camera->getContext();
        TS_ASSERT(context.getPtr() != nullptr);
    }

    void testPressButtonInvokesCurrentCallback() {
        m_camera->pressButton("execute");
        TS_ASSERT_EQUALS(config_calls, 1);
        TS_ASSERT_EQUALS(callback_calls, 1);
        TS_ASSERT_EQUALS(callback_camera, config_camera);
        TS_ASSERT_EQUALS(callback_context, config_context);
        TS_ASSERT_EQUALS(callback_widget_name, "execute");
        TS_ASSERT_EQUALS(callback_result, GP_OK);
    }

    void testInvalidButtonNamesFailBeforeCameraIo() {
        bool rejected = false;
        try {
            m_camera->pressButton("");
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);

        rejected = false;
        try {
            m_camera->pressButton(std::string("exe\0cute", 8));
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(config_calls, 0);
    }

    void testMovedFromCameraIsRejected() {
        gphoto2pp::CameraWrapper moved(std::move(*m_camera));
        bool rejected = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::CameraWrapperException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(config_calls, 0);
    }

    void testConfigurationFetchFailuresAreReported() {
        current_config_fetch_result = config_fetch_result::io_error;
        bool preserved_error = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(preserved_error);
        TS_ASSERT_EQUALS(callback_calls, 0);

        current_config_fetch_result = config_fetch_result::null_config;
        bool null_config = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::NullWidget &) {
            null_config = true;
        }
        TS_ASSERT(null_config);
        TS_ASSERT_EQUALS(callback_calls, 0);
    }

    void testMissingAmbiguousAndNonButtonNamesAreRejected() {
        current_layout = config_layout::missing_button;
        bool rejected = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);

        current_layout = config_layout::duplicate_button;
        rejected = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            rejected = true;
        }
        TS_ASSERT(rejected);
        TS_ASSERT_EQUALS(callback_calls, 0);

        current_layout = config_layout::non_button;
        bool wrong_type = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::InvalidWidgetType &) {
            wrong_type = true;
        }
        TS_ASSERT(wrong_type);
        TS_ASSERT_EQUALS(callback_calls, 0);
    }

    void testMissingCallbackAndCallbackErrorAreReported() {
        current_layout = config_layout::button_without_callback;
        bool missing_callback = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::NullWidget &) {
            missing_callback = true;
        }
        TS_ASSERT(missing_callback);

        current_layout = config_layout::single_button;
        callback_result = GP_ERROR_IO;
        bool preserved_error = false;
        try {
            m_camera->pressButton("execute");
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(preserved_error);
        TS_ASSERT_EQUALS(callback_calls, 1);
    }
};
