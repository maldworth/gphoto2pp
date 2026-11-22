#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_file_wrapper.hpp>

#include <vector>

class CameraFileWrapper_Data : public CxxTest::TestSuite {
public:
    void testGetDataAndSizeReturnsEmptyForEmptyFile() {
        gphoto2pp::CameraFileWrapper file;

        TS_ASSERT(file.getDataAndSize().empty());
        TS_ASSERT_EQUALS(file.getSize(), 0UL);
    }

    void testSetDataAndSizeCopiesBinaryDataAndCanClearIt() {
        gphoto2pp::CameraFileWrapper file;
        const std::vector<char> binary_data = {
            'a', '\0', 'b', static_cast<char>(0xff)
        };
        file.setDataAndSize(binary_data);
        TS_ASSERT_EQUALS(file.getDataAndSize(), binary_data);
        TS_ASSERT_EQUALS(file.getSize(), binary_data.size());

        file.setDataAndSize({});
        TS_ASSERT(file.getDataAndSize().empty());
        TS_ASSERT_EQUALS(file.getSize(), 0UL);
    }
};
