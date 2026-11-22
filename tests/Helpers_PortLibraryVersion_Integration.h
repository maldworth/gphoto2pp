#include <cxxtest/TestSuite.h>

#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-port-version.h>
}

#include <string>

class Helpers_PortLibraryVersion_Integration : public CxxTest::TestSuite {
public:
    void testLinkedLibraryVersion() {
        const char **version = gphoto2::gp_port_library_version(
            gphoto2::GP_VERSION_SHORT);
        TS_ASSERT(version != nullptr);
        if (version == nullptr || version[0] == nullptr) {
            TS_FAIL("libgphoto2_port did not report a version");
            return;
        }
        std::string expected;
        for (const char **line = version; *line != nullptr; ++line) {
            expected += *line;
            expected += '\n';
        }
        TS_ASSERT_EQUALS(gphoto2pp::PortLibraryVersion(), expected);
    }
};
