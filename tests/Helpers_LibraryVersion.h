#include <cxxtest/TestSuite.h>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-version.h>
#include <gphoto2/gphoto2-port-result.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

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
int port_locale_result = GP_OK;
std::string current_port_codeset = "UTF-8";
std::string previous_port_codeset;
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

extern "C" int gp_port_init_localedir(const char *localedir) {
    port_localedir_was_null = localedir == nullptr;
    initialized_port_localedir = localedir == nullptr ? "" : localedir;
    return port_locale_result;
}
}

class Helpers_LibraryVersion : public CxxTest::TestSuite {
public:
    void setUp() {
        null_port_version = false;
        null_camera_version = false;
        null_port_result = false;
        null_port_codeset = false;
        port_locale_result = GP_OK;
        current_port_codeset = "UTF-8";
        previous_port_codeset.clear();
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
