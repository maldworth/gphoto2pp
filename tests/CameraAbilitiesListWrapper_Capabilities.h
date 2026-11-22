#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_abilities_list_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-abilities-list.h>
#include <gphoto2/gphoto2-port-info-list.h>
#include <gphoto2/gphoto2-list.h>
#include <gphoto2/gphoto2-result.h>

struct _CameraAbilitiesList {
    int marker;
};
}

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <utility>

static_assert(!std::is_copy_constructible<
    gphoto2pp::CameraAbilitiesListWrapper>::value,
    "Camera abilities lists must not be copied");

namespace {
gphoto2::CameraAbilitiesList fake_abilities_list{};
int list_load_result = GP_OK;
int list_load_dir_result = GP_OK;
int list_get_result = GP_OK;
int list_append_result = GP_OK;
int list_free_calls = 0;
int list_load_dir_calls = 0;
int list_append_calls = 0;
std::string load_directory;
gphoto2::CameraAbilities appended{};
gphoto2::CameraAbilities fixture{};
}

namespace gphoto2 {
extern "C" int gp_abilities_list_new(CameraAbilitiesList **list) {
    *list = &fake_abilities_list;
    return GP_OK;
}

extern "C" int gp_abilities_list_free(CameraAbilitiesList *) {
    ++list_free_calls;
    return GP_OK;
}

extern "C" int gp_abilities_list_load(
    CameraAbilitiesList *, GPContext *) {
    return list_load_result;
}

extern "C" int gp_abilities_list_load_dir(
    CameraAbilitiesList *, const char *directory, GPContext *) {
    ++list_load_dir_calls;
    load_directory = directory;
    return list_load_dir_result;
}

extern "C" int gp_abilities_list_count(CameraAbilitiesList *) {
    return 1;
}

extern "C" int gp_abilities_list_lookup_model(
    CameraAbilitiesList *, const char *model) {
    return std::strcmp(model, "Synthetic Camera") == 0 ? 0 : GP_ERROR_BAD_PARAMETERS;
}

extern "C" int gp_abilities_list_get_abilities(
    CameraAbilitiesList *, int index, CameraAbilities *abilities) {
    if (index != 0) {
        return GP_ERROR_BAD_PARAMETERS;
    }
    if (list_get_result != GP_OK) {
        return list_get_result;
    }
    *abilities = fixture;
    return GP_OK;
}

extern "C" int gp_abilities_list_append(
    CameraAbilitiesList *, CameraAbilities abilities) {
    ++list_append_calls;
    appended = abilities;
    return list_append_result;
}

extern "C" int gp_abilities_list_reset(CameraAbilitiesList *) {
    return GP_OK;
}

extern "C" int gp_abilities_list_detect(
    CameraAbilitiesList *, GPPortInfoList *, CameraList *, GPContext *) {
    return GP_OK;
}
}

class CameraAbilitiesListWrapper_Capabilities : public CxxTest::TestSuite {
public:
    void setUp() {
        list_load_result = GP_OK;
        list_load_dir_result = GP_OK;
        list_get_result = GP_OK;
        list_append_result = GP_OK;
        list_free_calls = 0;
        list_load_dir_calls = 0;
        list_append_calls = 0;
        load_directory.clear();
        appended = {};
        fixture = {};
        std::snprintf(fixture.model, sizeof(fixture.model), "%s",
            "Synthetic Camera");
        fixture.status = gphoto2::GP_DRIVER_STATUS_EXPERIMENTAL;
        fixture.port = static_cast<gphoto2::GPPortType>(
            gphoto2::GP_PORT_USB | gphoto2::GP_PORT_PTPIP);
        fixture.speed[0] = 9600;
        fixture.speed[1] = 115200;
        fixture.operations = static_cast<gphoto2::CameraOperation>(
            gphoto2::GP_OPERATION_CAPTURE_IMAGE |
            gphoto2::GP_OPERATION_TRIGGER_CAPTURE);
        fixture.file_operations = static_cast<gphoto2::CameraFileOperation>(
            gphoto2::GP_FILE_OPERATION_DELETE |
            gphoto2::GP_FILE_OPERATION_RAW);
        fixture.folder_operations =
            static_cast<gphoto2::CameraFolderOperation>(
                gphoto2::GP_FOLDER_OPERATION_MAKE_DIR);
        fixture.usb_vendor = 0x1234;
        fixture.usb_product = 0x5678;
        fixture.usb_class = 6;
        fixture.usb_subclass = 1;
        fixture.usb_protocol = 1;
        std::snprintf(fixture.library, sizeof(fixture.library),
            "%s", "synthetic.so");
        std::snprintf(fixture.id, sizeof(fixture.id),
            "%s", "Synthetic ID");
        fixture.device_type = gphoto2::GP_DEVICE_STILL_CAMERA;
    }

    void testGetAbilitiesReturnsOwnedSnapshot() {
        gphoto2pp::CameraAbilitiesListWrapper list;
        const auto abilities = list.getAbilities(0);
        TS_ASSERT_EQUALS(abilities.model, "Synthetic Camera");
        TS_ASSERT_EQUALS(abilities.status,
            gphoto2pp::CameraDriverStatus::Experimental);
        TS_ASSERT_EQUALS(abilities.port,
            static_cast<std::uint32_t>(
                gphoto2::GP_PORT_USB | gphoto2::GP_PORT_PTPIP));
        TS_ASSERT_EQUALS(abilities.speeds.size(), 2U);
        TS_ASSERT_EQUALS(abilities.speeds[0], 9600);
        TS_ASSERT_EQUALS(abilities.speeds[1], 115200);
        TS_ASSERT_EQUALS(abilities.usbVendor, 0x1234);
        TS_ASSERT_EQUALS(abilities.usbProduct, 0x5678);
        TS_ASSERT_EQUALS(abilities.library, "synthetic.so");
        TS_ASSERT_EQUALS(abilities.id, "Synthetic ID");
    }

    void testGetAndLookupErrorsAndNulValidation() {
        gphoto2pp::CameraAbilitiesListWrapper list;
        TS_ASSERT_EQUALS(list.lookupModel("Synthetic Camera"), 0);
        std::string invalid = "Synthetic";
        invalid.push_back('\0');
        TS_ASSERT_THROWS(list.lookupModel(invalid),
            const gphoto2pp::exceptions::ArgumentException &);

        list_get_result = GP_ERROR_IO;
        bool preserved_error = false;
        try {
            list.getAbilities(0);
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(preserved_error);
        TS_ASSERT_THROWS(list.getAbilities(1),
            const gphoto2pp::exceptions::gphoto2_exception &);
    }

    void testLoadDirectoryAndAppendSnapshots() {
        gphoto2pp::CameraAbilitiesListWrapper list;
        list.loadDirectory("/synthetic/camlibs");
        TS_ASSERT_EQUALS(list_load_dir_calls, 1);
        TS_ASSERT_EQUALS(load_directory, "/synthetic/camlibs");

        gphoto2pp::CameraAbilitiesSnapshot abilities =
            list.getAbilities(0);
        list.append(abilities);
        TS_ASSERT_EQUALS(list_append_calls, 1);
        TS_ASSERT_EQUALS(std::string(appended.model),
            "Synthetic Camera");
        TS_ASSERT_EQUALS(appended.usb_vendor, 0x1234);

        abilities.model = std::string("bad\0model", 9);
        TS_ASSERT_THROWS(list.append(abilities),
            const gphoto2pp::exceptions::ArgumentException &);
    }

    void testMovedFromListIsRejected() {
        gphoto2pp::CameraAbilitiesListWrapper source;
        gphoto2pp::CameraAbilitiesListWrapper moved(std::move(source));
        TS_ASSERT_THROWS(source.count(), const std::logic_error &);
        TS_ASSERT_THROWS(source.getAbilities(0), const std::logic_error &);
        TS_ASSERT_EQUALS(moved.count(), 1);
    }
};
