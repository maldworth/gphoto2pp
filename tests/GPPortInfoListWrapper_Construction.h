#include <cxxtest/TestSuite.h>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

#include <utility>

namespace gphoto2 {
#include <gphoto2/gphoto2-port-info-list.h>
#include <gphoto2/gphoto2-result.h>

struct _GPPortInfoList {
    int marker;
};
}

namespace {
gphoto2::GPPortInfoList fake_list = {0};
gphoto2::GPPortInfoList second_fake_list = {1};
int new_result = GP_OK;
int load_result = GP_OK;
int free_result = GP_OK;
bool return_null = false;
int new_calls = 0;
int load_calls = 0;
int free_calls = 0;
gphoto2::GPPortInfoList *freed_list = nullptr;
}

namespace gphoto2 {
extern "C" int gp_port_info_list_new(GPPortInfoList **list) {
    ++new_calls;
    *list = return_null || new_result != GP_OK ? nullptr :
        (new_calls == 1 ? &fake_list : &second_fake_list);
    return new_result;
}

extern "C" int gp_port_info_list_load(GPPortInfoList *) {
    ++load_calls;
    return load_result;
}

extern "C" int gp_port_info_list_free(GPPortInfoList *list) {
    ++free_calls;
    freed_list = list;
    return free_result;
}
}

class GPPortInfoListWrapper_Construction : public CxxTest::TestSuite {
public:
    void setUp() {
        new_result = GP_OK;
        load_result = GP_OK;
        free_result = GP_OK;
        return_null = false;
        new_calls = 0;
        load_calls = 0;
        free_calls = 0;
        freed_list = nullptr;
    }

    void testLoadFailureFreesListAndPreservesError() {
        load_result = GP_ERROR_IO;
        bool preserved_error = false;
        try {
            gphoto2pp::GPPortInfoListWrapper ports;
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_IO;
        }
        TS_ASSERT(preserved_error);
        TS_ASSERT_EQUALS(load_calls, 1);
        TS_ASSERT_EQUALS(free_calls, 1);
        TS_ASSERT_EQUALS(freed_list, &fake_list);
    }

    void testSuccessfulConstructionFreesListOnDestruction() {
        {
            gphoto2pp::GPPortInfoListWrapper ports;
            TS_ASSERT_EQUALS(ports.getPtr(), &fake_list);
            TS_ASSERT_EQUALS(free_calls, 0);
        }
        TS_ASSERT_EQUALS(load_calls, 1);
        TS_ASSERT_EQUALS(free_calls, 1);
        TS_ASSERT_EQUALS(freed_list, &fake_list);
    }

    void testAllocationFailureDoesNotLoadOrFree() {
        new_result = GP_ERROR_NO_MEMORY;
        bool preserved_error = false;
        try {
            gphoto2pp::GPPortInfoListWrapper ports;
        } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
            preserved_error = error.getResultCode() == GP_ERROR_NO_MEMORY;
        }
        TS_ASSERT(preserved_error);
        TS_ASSERT_EQUALS(load_calls, 0);
        TS_ASSERT_EQUALS(free_calls, 0);
    }

    void testNullListIsRejectedBeforeLoading() {
        return_null = true;
        TS_ASSERT_THROWS(gphoto2pp::GPPortInfoListWrapper ports,
            const std::runtime_error &);
        TS_ASSERT_EQUALS(load_calls, 0);
        TS_ASSERT_EQUALS(free_calls, 0);
    }

    void testMoveAssignmentReleasesPreviousListOnce() {
        {
            gphoto2pp::GPPortInfoListWrapper source;
            gphoto2pp::GPPortInfoListWrapper destination;
            destination = std::move(source);
            TS_ASSERT_EQUALS(source.getPtr(), nullptr);
            TS_ASSERT_EQUALS(destination.getPtr(), &fake_list);
            TS_ASSERT_EQUALS(free_calls, 1);
            TS_ASSERT_EQUALS(freed_list, &second_fake_list);

            destination = std::move(destination);
            TS_ASSERT_EQUALS(destination.getPtr(), &fake_list);
            TS_ASSERT_EQUALS(free_calls, 1);
        }
        TS_ASSERT_EQUALS(free_calls, 2);
        TS_ASSERT_EQUALS(freed_list, &fake_list);
    }

    void testMoveAssignmentReportsReleaseFailure() {
        {
            gphoto2pp::GPPortInfoListWrapper source;
            gphoto2pp::GPPortInfoListWrapper destination;
            free_result = GP_ERROR_IO;
            bool preserved_error = false;
            try {
                destination = std::move(source);
            } catch (const gphoto2pp::exceptions::gphoto2_exception &error) {
                preserved_error = error.getResultCode() == GP_ERROR_IO;
            }
            TS_ASSERT(preserved_error);
            TS_ASSERT_EQUALS(source.getPtr(), &fake_list);
            TS_ASSERT_EQUALS(destination.getPtr(), &second_fake_list);
            free_result = GP_OK;
        }
        TS_ASSERT_EQUALS(free_calls, 3);
    }
};
