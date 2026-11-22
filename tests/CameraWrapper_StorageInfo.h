#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_storage_info.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>

namespace {
enum class storage_result {
    populated, empty, unsupported, io_error, negative_count, missing_array,
    bad_base, bad_label, bad_description
};

storage_result next_storage_result = storage_result::populated;
gphoto2::GPPortInfo fake_port = nullptr;
}

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
    std::snprintf(abilities->model, sizeof(abilities->model),
        "%s", "Synthetic camera");
    return GP_OK;
}

extern "C" int gp_camera_get_storageinfo(
    Camera *, CameraStorageInformation **info, int *count, GPContext *) {
    if (next_storage_result == storage_result::empty) {
        return GP_OK;
    }
    if (next_storage_result == storage_result::negative_count) {
        *count = -1;
        return GP_OK;
    }
    if (next_storage_result == storage_result::missing_array) {
        *count = 1;
        return GP_OK;
    }
    *info = static_cast<CameraStorageInformation *>(
        std::calloc(2, sizeof(CameraStorageInformation)));
    if (*info == nullptr) {
        return GP_ERROR_NO_MEMORY;
    }
    if (next_storage_result == storage_result::unsupported) {
        return GP_ERROR_NOT_SUPPORTED;
    }
    if (next_storage_result == storage_result::io_error) {
        return GP_ERROR_IO;
    }

    *count = 2;
    CameraStorageInformation &first = (*info)[0];
    first.fields = static_cast<CameraStorageInfoFields>(
        GP_STORAGEINFO_BASE | GP_STORAGEINFO_LABEL | GP_STORAGEINFO_DESCRIPTION |
        GP_STORAGEINFO_ACCESS | GP_STORAGEINFO_STORAGETYPE |
        GP_STORAGEINFO_FILESYSTEMTYPE | GP_STORAGEINFO_MAXCAPACITY |
        GP_STORAGEINFO_FREESPACEKBYTES | GP_STORAGEINFO_FREESPACEIMAGES);
    std::snprintf(first.basedir, sizeof(first.basedir), "%s", "/store");
    std::snprintf(first.label, sizeof(first.label), "%s", "Card");
    std::snprintf(first.description, sizeof(first.description), "%s", "Primary");
    first.type = GP_STORAGEINFO_ST_REMOVABLE_RAM;
    first.fstype = GP_STORAGEINFO_FST_DCF;
    first.access = GP_STORAGEINFO_AC_READONLY_WITH_DELETE;
    first.capacitykbytes = std::uint64_t{1} << 40;
    first.freekbytes = 123;
    first.freeimages = 7;

    CameraStorageInformation &second = (*info)[1];
    second.fields = GP_STORAGEINFO_LABEL;
    std::memset(second.basedir, 'x', sizeof(second.basedir));
    second.capacitykbytes = 999;

    if (next_storage_result == storage_result::bad_base) {
        std::memset(first.basedir, 'x', sizeof(first.basedir));
    } else if (next_storage_result == storage_result::bad_label) {
        std::memset(first.label, 'x', sizeof(first.label));
    } else if (next_storage_result == storage_result::bad_description) {
        std::memset(first.description, 'x', sizeof(first.description));
    }
    return GP_OK;
}
}

class CameraWrapper_StorageInfo : public CxxTest::TestSuite {
    gphoto2::GPPortInfoList *m_ports = nullptr;
    std::unique_ptr<gphoto2pp::CameraWrapper> m_camera;

public:
    void setUp() {
        next_storage_result = storage_result::populated;
        if (gphoto2::gp_port_info_list_new(&m_ports) != GP_OK ||
            gphoto2::gp_port_info_new(&fake_port) != GP_OK ||
            gphoto2::gp_port_info_set_name(fake_port, "Synthetic storage port") != GP_OK ||
            gphoto2::gp_port_info_set_type(fake_port, gphoto2::GP_PORT_USB) != GP_OK ||
            gphoto2::gp_port_info_set_path(fake_port, "fake:no-usb") != GP_OK ||
            gphoto2::gp_port_info_list_append(m_ports, fake_port) < GP_OK) {
            throw std::runtime_error("Unable to configure synthetic storage port");
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

    void testOwnedPopulatedAndPartialEntries() {
        using namespace gphoto2pp;
        const auto storage = m_camera->getStorageInfo();
        m_camera.reset();
        TS_ASSERT_EQUALS(storage.size(), 2U);
        if (storage.size() != 2) {
            return;
        }
        const auto &first = storage[0];
        TS_ASSERT(first.has(CameraStorageInfoField::Base));
        TS_ASSERT(first.has(CameraStorageInfoField::Label));
        TS_ASSERT(first.has(CameraStorageInfoField::Description));
        TS_ASSERT(first.has(CameraStorageInfoField::StorageType));
        TS_ASSERT(first.has(CameraStorageInfoField::FilesystemType));
        TS_ASSERT(first.has(CameraStorageInfoField::Access));
        TS_ASSERT(first.has(CameraStorageInfoField::MaxCapacity));
        TS_ASSERT(first.has(CameraStorageInfoField::FreeSpaceKBytes));
        TS_ASSERT(first.has(CameraStorageInfoField::FreeSpaceImages));
        TS_ASSERT_EQUALS(first.baseDirectory, "/store");
        TS_ASSERT_EQUALS(first.label, "Card");
        TS_ASSERT_EQUALS(first.description, "Primary");
        TS_ASSERT_EQUALS(first.storageType, CameraStorageType::RemovableRam);
        TS_ASSERT_EQUALS(first.filesystemType, CameraStorageFilesystemType::Dcf);
        TS_ASSERT_EQUALS(first.access, CameraStorageAccessType::ReadOnlyWithDelete);
        TS_ASSERT_EQUALS(first.capacityKBytes, std::uint64_t{1} << 40);
        TS_ASSERT_EQUALS(first.freeKBytes, 123U);
        TS_ASSERT_EQUALS(first.freeImages, 7U);

        TS_ASSERT(storage[1].has(CameraStorageInfoField::Label));
        TS_ASSERT(storage[1].label.empty());
        TS_ASSERT(!storage[1].has(CameraStorageInfoField::Base));
        TS_ASSERT(storage[1].baseDirectory.empty());
        TS_ASSERT(!storage[1].has(CameraStorageInfoField::MaxCapacity));
        TS_ASSERT_EQUALS(storage[1].capacityKBytes, 0U);
    }

    void testEmptyAndDriverErrors() {
        next_storage_result = storage_result::empty;
        TS_ASSERT(m_camera->getStorageInfo().empty());
        const storage_result results[] = {
            storage_result::unsupported, storage_result::io_error
        };
        const int errors[] = {GP_ERROR_NOT_SUPPORTED, GP_ERROR_IO};
        for (int i = 0; i < 2; ++i) {
            next_storage_result = results[i];
            bool preserved = false;
            try {
                m_camera->getStorageInfo();
            } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
                preserved = error.getResultCode() == errors[i];
            }
            TS_ASSERT(preserved);
        }
    }

    void testMalformedDataIsRejected() {
        const storage_result results[] = {
            storage_result::negative_count, storage_result::missing_array,
            storage_result::bad_base, storage_result::bad_label,
            storage_result::bad_description
        };
        for (const auto result : results) {
            next_storage_result = result;
            TS_ASSERT_THROWS(m_camera->getStorageInfo(),
                const std::runtime_error &);
        }
    }
};
