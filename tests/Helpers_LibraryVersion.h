#include <cxxtest/TestSuite.h>

#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-version.h>
#include <gphoto2/gphoto2-port-result.h>
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
}

class Helpers_LibraryVersion : public CxxTest::TestSuite {
public:
    void setUp() {
        null_port_version = false;
        null_camera_version = false;
        null_port_result = false;
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
};
