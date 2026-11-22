#include <cxxtest/TestSuite.h>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-port-info-list.h>
#include <gphoto2/gphoto2-result.h>
}

#include <stdexcept>
#include <type_traits>
#include <utility>

static_assert(!std::is_copy_constructible<
    gphoto2pp::GPPortInfoListWrapper>::value, "Port lists must not be copied");
static_assert(!std::is_copy_assignable<
    gphoto2pp::GPPortInfoListWrapper>::value, "Port lists must not be copied");

namespace {

int append_synthetic_port(
    gphoto2pp::GPPortInfoListWrapper &ports, gphoto2::GPPortType type) {
    gphoto2::GPPortInfo info = nullptr;
    if (gphoto2::gp_port_info_new(&info) != GP_OK ||
        gphoto2::gp_port_info_set_name(info, "Synthetic test port") != GP_OK ||
        gphoto2::gp_port_info_set_path(info, "test:camera") != GP_OK ||
        gphoto2::gp_port_info_set_type(info, type) != GP_OK) {
        throw std::runtime_error("Unable to create synthetic port information");
    }

    const int index = gphoto2::gp_port_info_list_append(ports.getPtr(), info);
    if (index < 0) {
        throw std::runtime_error("Unable to append synthetic port information");
    }
    return index;
}

}  // namespace

class GPPortInfoListWrapper_Info : public CxxTest::TestSuite {
public:
    void testGetInfoReturnsOwnedSnapshot() {
        gphoto2pp::PortInfoSnapshot info;
        {
            gphoto2pp::GPPortInfoListWrapper ports;
            const int index = append_synthetic_port(
                ports, gphoto2::GP_PORT_SERIAL);
            info = ports.getInfo(index);
        }

        TS_ASSERT_EQUALS(info.name, "Synthetic test port");
        TS_ASSERT_EQUALS(info.path, "test:camera");
        TS_ASSERT_EQUALS(info.type, gphoto2pp::GPPortTypeWrapper::Serial);
    }

    void testUnknownPortTypeValueIsPreserved() {
        gphoto2pp::GPPortInfoListWrapper ports;
        const auto unknown_type =
            static_cast<gphoto2::GPPortType>(1 << 8);
        const int index = append_synthetic_port(ports, unknown_type);

        const auto info = ports.getInfo(index);
        TS_ASSERT_EQUALS(static_cast<int>(info.type),
            static_cast<int>(unknown_type));
    }

    void testLookupByPathAndName() {
        gphoto2pp::GPPortInfoListWrapper ports;
        const int expected_index = append_synthetic_port(
            ports, gphoto2::GP_PORT_SERIAL);

        TS_ASSERT_EQUALS(ports.lookupPath("test:camera"), expected_index);
        TS_ASSERT_EQUALS(
            ports.lookupName("Synthetic test port"), expected_index);
    }

    void testMovesTransferUsableListAndRejectMovedFromOperations() {
        gphoto2pp::GPPortInfoListWrapper source;
        const int index = append_synthetic_port(source, gphoto2::GP_PORT_SERIAL);
        gphoto2pp::GPPortInfoListWrapper moved(std::move(source));

        TS_ASSERT_EQUALS(source.getPtr(), nullptr);
        TS_ASSERT_THROWS(source.count(), const std::logic_error &);
        TS_ASSERT_THROWS(source.lookupName("name"), const std::logic_error &);
        TS_ASSERT_THROWS(source.lookupPath("path"), const std::logic_error &);
        TS_ASSERT_THROWS(source.getInfo(0), const std::logic_error &);
        TS_ASSERT_EQUALS(moved.getInfo(index).path, "test:camera");

        source = std::move(moved);
        TS_ASSERT_EQUALS(moved.getPtr(), nullptr);
        TS_ASSERT_EQUALS(source.getInfo(index).name, "Synthetic test port");
    }

    void testLookupsRejectEmbeddedNulBytes() {
        gphoto2pp::GPPortInfoListWrapper ports;
        std::string invalid_path = "test:camera";
        invalid_path.push_back('\0');
        invalid_path += "suffix";
        bool invalid_path_rejected = false;
        try {
            ports.lookupPath(invalid_path);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            invalid_path_rejected = true;
        }
        TS_ASSERT(invalid_path_rejected);

        std::string invalid_name = "Synthetic test port";
        invalid_name.push_back('\0');
        invalid_name += "suffix";
        bool invalid_name_rejected = false;
        try {
            ports.lookupName(invalid_name);
        } catch (const gphoto2pp::exceptions::ArgumentException &) {
            invalid_name_rejected = true;
        }
        TS_ASSERT(invalid_name_rejected);
    }

    void testInvalidIndicesPreserveLibgphotoError() {
        gphoto2pp::GPPortInfoListWrapper ports;
        const int invalid_indices[] = {-1, ports.count()};
        for (const int index : invalid_indices) {
            bool preserved_error = false;
            try {
                ports.getInfo(index);
            } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
                preserved_error = error.getResultCode() == GP_ERROR_BAD_PARAMETERS;
            }
            TS_ASSERT(preserved_error);
        }
    }
};
