#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
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

    void testClearAndCopyFromMemoryFiles() {
        gphoto2pp::CameraFileWrapper source;
        source.setDataAndSize({'s', 'r', 'c'});
        source.setFileName("source.jpg");
        gphoto2pp::CameraFileWrapper destination;
        destination.setDataAndSize({'o', 'l', 'd'});
        destination.copyFrom(source);

        TS_ASSERT_EQUALS(destination.getDataAndSize(),
            (std::vector<char>{'s', 'r', 'c'}));
        TS_ASSERT_EQUALS(destination.getFileName(), "source.jpg");

        destination.clear();
        TS_ASSERT(destination.getDataAndSize().empty());
        TS_ASSERT_EQUALS(destination.getFileName(), "");
    }

    void testFdConstructorTransfersOwnership() {
        char path[] = "/tmp/gphoto2pp-file-XXXXXX";
        const int fd = mkstemp(path);
        TS_ASSERT(fd >= 0);
        if (fd < 0) {
            return;
        }
        const char data[] = "abc";
        TS_ASSERT_EQUALS(write(fd, data, 3), 3);
        TS_ASSERT(lseek(fd, 0, SEEK_SET) == 0);

        {
            gphoto2pp::CameraFileWrapper file(fd);
            TS_ASSERT_EQUALS(file.getSize(), 3UL);
        }

        TS_ASSERT_EQUALS(fcntl(fd, F_GETFD), -1);
        TS_ASSERT_EQUALS(errno, EBADF);
        unlink(path);
    }

    void testNegativeFdIsRejected() {
        TS_ASSERT_THROWS(gphoto2pp::CameraFileWrapper(-1),
            const gphoto2pp::exceptions::ArgumentException &);
    }
};
