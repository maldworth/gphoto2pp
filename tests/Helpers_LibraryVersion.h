#include <cxxtest/TestSuite.h>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-version.h>
#include <gphoto2/gphoto2-port-result.h>
#include <gphoto2/gphoto2-port-info-list.h>
#include <gphoto2/gphoto2-abilities-list.h>
#include <gphoto2/gphoto2-setting.h>
#include <gphoto2/gphoto2-result.h>
}

#include <algorithm>
#include <array>
#include <cstring>
#include <stdexcept>
#include <string>

namespace {
char port_version[] = "port 1";
const char *port_short[] = {port_version, nullptr};
const char *port_verbose[] = {"port 1", "backend detail", nullptr};
const char *camera_short[] = {"camera 2", nullptr};
const char *camera_verbose[] = {"camera 2", "build detail", nullptr};
bool null_port_version = false;
bool null_camera_version = false;
bool null_port_result = false;
bool null_port_codeset = false;
bool null_message_codeset = false;
int locale_result = GP_OK;
int port_locale_result = GP_OK;
std::string initialized_localedir;
bool localedir_was_null = false;
int setting_get_result = GP_OK;
int setting_set_result = GP_OK;
bool setting_get_unterminated = false;
std::string setting_value = "stored";
std::string setting_id;
std::string setting_key;
std::string setting_set_value;
gphoto2::gp_settings_func setting_get_callback = nullptr;
gphoto2::gp_settings_func setting_set_callback = nullptr;
void *setting_get_callback_data = nullptr;
void *setting_set_callback_data = nullptr;
std::string current_port_codeset = "UTF-8";
std::string previous_port_codeset;
std::string current_message_codeset = "UTF-8";
std::string initialized_port_localedir;
bool port_localedir_was_null = false;
}

namespace gphoto2 {
extern "C" const char **gp_port_library_version(GPVersionVerbosity verbose) {
    if (null_port_version) {
        return nullptr;
    }
    return verbose == GP_VERSION_VERBOSE ? port_verbose : port_short;
}

extern "C" const char **gp_library_version(GPVersionVerbosity verbose) {
    if (null_camera_version) {
        return nullptr;
    }
    return verbose == GP_VERSION_VERBOSE ? camera_verbose : camera_short;
}

extern "C" const char *gp_port_result_as_string(int result) {
    if (null_port_result) {
        return nullptr;
    }
    return result == -7 ? "I/O problem" : "Unknown result";
}

extern "C" const char *gp_port_message_codeset(const char *codeset) {
    if (null_port_codeset) {
        return nullptr;
    }
    previous_port_codeset = current_port_codeset;
    if (codeset != nullptr) {
        current_port_codeset = codeset;
    }
    return previous_port_codeset.c_str();
}

extern "C" const char *gp_message_codeset(const char *codeset) {
    if (null_message_codeset) {
        return nullptr;
    }
    if (codeset != nullptr) {
        current_message_codeset = codeset;
    }
    return current_message_codeset.c_str();
}

extern "C" int gp_port_init_localedir(const char *localedir) {
    port_localedir_was_null = localedir == nullptr;
    initialized_port_localedir = localedir == nullptr ? "" : localedir;
    return port_locale_result;
}

extern "C" int gp_init_localedir(const char *localedir) {
    localedir_was_null = localedir == nullptr;
    initialized_localedir = localedir == nullptr ? "" : localedir;
    return locale_result;
}

extern "C" int gp_setting_get(char *id, char *key, char *value) {
    setting_id = id;
    setting_key = key;
    if (setting_get_unterminated) {
        std::fill(value, value + 256, 'x');
    } else {
        std::copy(setting_value.begin(), setting_value.end(), value);
        value[setting_value.size()] = '\0';
    }
    return setting_get_result;
}

extern "C" int gp_setting_set(char *id, char *key, char *value) {
    setting_id = id;
    setting_key = key;
    setting_set_value = value;
    return setting_set_result;
}

extern "C" void gp_setting_set_get_func(
    gp_settings_func callback, void *data) {
    setting_get_callback = callback;
    setting_get_callback_data = data;
}

extern "C" void gp_setting_set_set_func(
    gp_settings_func callback, void *data) {
    setting_set_callback = callback;
    setting_set_callback_data = data;
}
}

class Helpers_LibraryVersion : public CxxTest::TestSuite {
public:
    void setUp() {
        null_port_version = false;
        null_camera_version = false;
        null_port_result = false;
        null_port_codeset = false;
        null_message_codeset = false;
        locale_result = GP_OK;
        port_locale_result = GP_OK;
        setting_get_result = GP_OK;
        setting_set_result = GP_OK;
        setting_get_unterminated = false;
        setting_value = "stored";
        setting_id.clear();
        setting_key.clear();
        setting_set_value.clear();
#ifdef GPHOTO2PP_HAVE_SETTING_CALLBACKS
        gphoto2pp::SetSettingGetCallback({});
        gphoto2pp::SetSettingSetCallback({});
#endif
        initialized_localedir.clear();
        localedir_was_null = false;
        current_port_codeset = "UTF-8";
        previous_port_codeset.clear();
        current_message_codeset = "UTF-8";
        initialized_port_localedir.clear();
        port_localedir_was_null = false;
        port_version[0] = 'p';
    }

    void testPortVersionVerbosityAndOwnership() {
        const std::string version = gphoto2pp::PortLibraryVersion();
        TS_ASSERT_EQUALS(version, "port 1\n");
        TS_ASSERT_EQUALS(gphoto2pp::PortLibraryVersion(true),
            "port 1\nbackend detail\n");
        port_version[0] = 'X';
        TS_ASSERT_EQUALS(version, "port 1\n");
    }

    void testExistingCameraVersionFormatIsPreserved() {
        TS_ASSERT_EQUALS(gphoto2pp::LibraryVersion(), "camera 2\n");
        TS_ASSERT_EQUALS(gphoto2pp::LibraryVersion(true),
            "camera 2\nbuild detail\n");
    }

    void testPortResultTextAndNullHandling() {
        TS_ASSERT_EQUALS(gphoto2pp::PortResultString(-7),
            "I/O problem");
        TS_ASSERT_EQUALS(gphoto2pp::PortResultString(123),
            "Unknown result");
        null_port_result = true;
        TS_ASSERT_THROWS(gphoto2pp::PortResultString(-7),
            const std::runtime_error &);
    }

    void testNullVersionArraysAreReported() {
        null_port_version = true;
        TS_ASSERT_THROWS(gphoto2pp::PortLibraryVersion(),
            const std::runtime_error &);
        null_camera_version = true;
        TS_ASSERT_THROWS(gphoto2pp::LibraryVersion(),
            const std::runtime_error &);
    }

    void testPortMessageCodesetCanBeSetAndQueried() {
        TS_ASSERT_EQUALS(gphoto2pp::PortMessageCodeset("ASCII"), "UTF-8");
        TS_ASSERT_EQUALS(gphoto2pp::PortMessageCodeset(), "ASCII");

        std::string invalid_codeset = "UTF-8";
        invalid_codeset.push_back('\0');
        invalid_codeset += "suffix";
        TS_ASSERT_THROWS(gphoto2pp::PortMessageCodeset(invalid_codeset),
            const gphoto2pp::exceptions::ArgumentException &);

        null_port_codeset = true;
        TS_ASSERT_THROWS(gphoto2pp::PortMessageCodeset(),
            const std::runtime_error &);
    }

    void testMessageCodesetCanBeSetAndQueried() {
#ifdef GPHOTO2PP_HAVE_MESSAGE_CODESET
        TS_ASSERT_EQUALS(gphoto2pp::MessageCodeset("ASCII"), "ASCII");
        TS_ASSERT_EQUALS(gphoto2pp::MessageCodeset(), "ASCII");

        std::string invalid_codeset = "UTF-8";
        invalid_codeset.push_back('\0');
        invalid_codeset += "suffix";
        TS_ASSERT_THROWS(gphoto2pp::MessageCodeset(invalid_codeset),
            const gphoto2pp::exceptions::ArgumentException &);

        null_message_codeset = true;
        TS_ASSERT_THROWS(gphoto2pp::MessageCodeset(),
            const std::runtime_error &);
#else
        TS_ASSERT_THROWS(gphoto2pp::MessageCodeset(),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
        TS_ASSERT_THROWS(gphoto2pp::MessageCodeset("UTF-8"),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
#endif
    }

    void testLocaleDirectoryInitializationAndErrors() {
#ifdef GPHOTO2PP_HAVE_INIT_LOCALEDIR
        gphoto2pp::InitializeLocaleDirectory("/tmp/gphoto-locale");
        TS_ASSERT_EQUALS(initialized_localedir, "/tmp/gphoto-locale");
        TS_ASSERT(!localedir_was_null);

        gphoto2pp::InitializeLocaleDirectory();
        TS_ASSERT(localedir_was_null);

        std::string invalid_directory = "/tmp/locale";
        invalid_directory.push_back('\0');
        invalid_directory += "suffix";
        TS_ASSERT_THROWS(
            gphoto2pp::InitializeLocaleDirectory(invalid_directory),
            const gphoto2pp::exceptions::ArgumentException &);

        locale_result = GP_ERROR_NO_MEMORY;
        bool preserved_error = false;
        try {
            gphoto2pp::InitializeLocaleDirectory("/tmp/other");
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error =
                error.getResultCode() == GP_ERROR_NO_MEMORY;
        }
        TS_ASSERT(preserved_error);
#else
        std::string invalid_directory = "/tmp/locale";
        invalid_directory.push_back('\0');
        invalid_directory += "suffix";
        TS_ASSERT_THROWS(
            gphoto2pp::InitializeLocaleDirectory(invalid_directory),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(gphoto2pp::InitializeLocaleDirectory(),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
        TS_ASSERT_THROWS(
            gphoto2pp::InitializeLocaleDirectory("/tmp/gphoto-locale"),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
#endif
    }

    void testGetSettingValidatesAndCopiesResult() {
#ifdef GPHOTO2PP_HAVE_SETTING_GET
        TS_ASSERT_EQUALS(gphoto2pp::GetSetting("frontend", "mode"), "stored");
        TS_ASSERT_EQUALS(setting_id, "frontend");
        TS_ASSERT_EQUALS(setting_key, "mode");

        setting_value = std::string(255, 'v');
        TS_ASSERT_EQUALS(gphoto2pp::GetSetting("frontend", "mode").size(),
            255u);

        setting_get_result = GP_ERROR_NO_MEMORY;
        bool preserved_error = false;
        try {
            gphoto2pp::GetSetting("frontend", "mode");
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error =
                error.getResultCode() == GP_ERROR_NO_MEMORY;
        }
        TS_ASSERT(preserved_error);

        setting_get_result = GP_OK;
        setting_get_unterminated = true;
        TS_ASSERT_THROWS(gphoto2pp::GetSetting("frontend", "mode"),
            const std::runtime_error &);
#else
        TS_ASSERT_THROWS(gphoto2pp::GetSetting("frontend", "mode"),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
#endif
    }

    void testSetSettingValidatesInputsAndPropagatesErrors() {
#ifdef GPHOTO2PP_HAVE_SETTING_SET
        gphoto2pp::SetSetting("frontend", "mode", "manual");
        TS_ASSERT_EQUALS(setting_id, "frontend");
        TS_ASSERT_EQUALS(setting_key, "mode");
        TS_ASSERT_EQUALS(setting_set_value, "manual");

        gphoto2pp::SetSetting(
            "frontend", "mode", std::string(255, 'x'));
        TS_ASSERT_EQUALS(setting_set_value.size(), 255u);

        setting_set_result = GP_ERROR_NO_MEMORY;
        bool preserved_error = false;
        try {
            gphoto2pp::SetSetting("frontend", "mode", "auto");
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error =
                error.getResultCode() == GP_ERROR_NO_MEMORY;
        }
        TS_ASSERT(preserved_error);
#else
        TS_ASSERT_THROWS(
            gphoto2pp::SetSetting("frontend", "mode", "manual"),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
#endif

        std::string invalid = "mode";
        invalid.push_back('\0');
        invalid += "suffix";
        TS_ASSERT_THROWS(gphoto2pp::GetSetting("frontend", invalid),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(
            gphoto2pp::SetSetting("frontend", "mode", invalid),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(
            gphoto2pp::SetSetting("frontend", "mode", std::string(256, 'x')),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(
            gphoto2pp::GetSetting(std::string(256, 'i'), "mode"),
            const gphoto2pp::exceptions::ArgumentException &);
    }

    void testSettingCallbacksAreBoundedAndContainExceptions() {
#ifdef GPHOTO2PP_HAVE_SETTING_CALLBACKS
        gphoto2pp::SetSettingGetCallback(
            [](std::string const &id, std::string const &key,
                std::string &value) {
                setting_id = id;
                setting_key = key;
                value = std::string(255, 'v');
                return GP_OK;
            });
        TS_ASSERT(setting_get_callback != nullptr);

        char id[] = "frontend";
        char key[] = "mode";
        std::array<char, 256> output;
        output.fill('?');
        TS_ASSERT_EQUALS(setting_get_callback(
            id, key, output.data(), setting_get_callback_data), GP_OK);
        TS_ASSERT_EQUALS(setting_id, "frontend");
        TS_ASSERT_EQUALS(setting_key, "mode");
        TS_ASSERT_EQUALS(std::string(output.data()), std::string(255, 'v'));

        gphoto2pp::SetSettingGetCallback(
            [](std::string const &, std::string const &,
                std::string &value) {
                value = "not written on error";
                return GP_ERROR_NO_MEMORY;
            });
        output.fill('?');
        TS_ASSERT_EQUALS(setting_get_callback(
            id, key, output.data(), setting_get_callback_data),
            GP_ERROR_NO_MEMORY);
        TS_ASSERT_EQUALS(output[0], '?');

        gphoto2pp::SetSettingGetCallback(
            [](std::string const &, std::string const &,
                std::string &value) {
                value = std::string(256, 'x');
                return GP_OK;
            });
        output.fill('?');
        TS_ASSERT_EQUALS(setting_get_callback(
            id, key, output.data(), setting_get_callback_data),
            GP_ERROR_BAD_PARAMETERS);
        TS_ASSERT_EQUALS(output[0], '?');

        gphoto2pp::SetSettingGetCallback(
            [](std::string const &, std::string const &, std::string &) -> int {
                throw std::runtime_error("synthetic getter failure");
            });
        TS_ASSERT_EQUALS(setting_get_callback(
            id, key, output.data(), setting_get_callback_data), GP_ERROR);
        gphoto2pp::SetSettingGetCallback({});
        TS_ASSERT(setting_get_callback == nullptr);

        gphoto2pp::SetSettingSetCallback(
            [](std::string const &settingId, std::string const &settingKey,
                std::string const &value) {
                setting_id = settingId;
                setting_key = settingKey;
                setting_set_value = value;
                return GP_OK;
            });
        char value[] = "custom";
        TS_ASSERT_EQUALS(setting_set_callback(
            id, key, value, setting_set_callback_data), GP_OK);
        TS_ASSERT_EQUALS(setting_id, "frontend");
        TS_ASSERT_EQUALS(setting_key, "mode");
        TS_ASSERT_EQUALS(setting_set_value, "custom");

        gphoto2pp::SetSettingSetCallback(
            [](std::string const &, std::string const &,
                std::string const &) {
                return GP_ERROR_NO_MEMORY;
            });
        TS_ASSERT_EQUALS(setting_set_callback(
            id, key, value, setting_set_callback_data), GP_ERROR_NO_MEMORY);

        char oversized_value[257];
        std::fill(oversized_value, oversized_value + 256, 'x');
        oversized_value[256] = '\0';
        TS_ASSERT_EQUALS(setting_set_callback(
            id, key, oversized_value, setting_set_callback_data),
            GP_ERROR_BAD_PARAMETERS);

        gphoto2pp::SetSettingSetCallback(
            [](std::string const &, std::string const &,
                std::string const &) -> int {
                throw std::runtime_error("synthetic setter failure");
            });
        TS_ASSERT_EQUALS(setting_set_callback(
            id, key, value, setting_set_callback_data), GP_ERROR);
        gphoto2pp::SetSettingSetCallback({});
        TS_ASSERT(setting_set_callback == nullptr);
#else
        TS_ASSERT_THROWS(
            gphoto2pp::SetSettingGetCallback({}),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
        TS_ASSERT_THROWS(
            gphoto2pp::SetSettingSetCallback({}),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
#endif
    }

    void testPortLocaleDirectoryInitializationAndErrors() {
#ifdef GPHOTO2PP_HAVE_PORT_INIT_LOCALEDIR
        gphoto2pp::InitializePortLocaleDirectory("/tmp/gphoto-locale");
        TS_ASSERT_EQUALS(initialized_port_localedir, "/tmp/gphoto-locale");
        TS_ASSERT(!port_localedir_was_null);

        gphoto2pp::InitializePortLocaleDirectory();
        TS_ASSERT(port_localedir_was_null);

        std::string invalid_directory = "/tmp/locale";
        invalid_directory.push_back('\0');
        invalid_directory += "suffix";
        TS_ASSERT_THROWS(
            gphoto2pp::InitializePortLocaleDirectory(invalid_directory),
            const gphoto2pp::exceptions::ArgumentException &);

        port_locale_result = GP_ERROR_NO_MEMORY;
        bool preserved_error = false;
        try {
            gphoto2pp::InitializePortLocaleDirectory("/tmp/other");
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error =
                error.getResultCode() == GP_ERROR_NO_MEMORY;
        }
        TS_ASSERT(preserved_error);
#else
        std::string invalid_directory = "/tmp/locale";
        invalid_directory.push_back('\0');
        invalid_directory += "suffix";
        TS_ASSERT_THROWS(
            gphoto2pp::InitializePortLocaleDirectory(invalid_directory),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(gphoto2pp::InitializePortLocaleDirectory(),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
        TS_ASSERT_THROWS(
            gphoto2pp::InitializePortLocaleDirectory("/tmp/gphoto-locale"),
            const gphoto2pp::exceptions::InvalidLinkedVersionException &);
#endif
    }
};
