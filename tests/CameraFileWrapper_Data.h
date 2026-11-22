#include <cxxtest/TestSuite.h>

#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-result.h>
}

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
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

    void testHandlerCallbacksReceiveSizeReadAndWrite() {
        std::vector<char> written;
        gphoto2pp::CameraFileHandlerCallbacks callbacks;
        callbacks.size = [](std::uint64_t &size) {
            size = 3;
            return GP_OK;
        };
        callbacks.read = [](std::vector<char> &data,
            std::uint64_t &length) {
            data = {'a', 'b', 'c'};
            length = data.size();
            return GP_OK;
        };
        callbacks.write = [&written](std::vector<char> const &data,
            std::uint64_t &length) {
            written = data;
            length = data.size();
            return GP_OK;
        };

        gphoto2pp::CameraFileWrapper file(callbacks);
        TS_ASSERT_EQUALS(file.getSize(), 3UL);
        TS_ASSERT_EQUALS(file.getDataAndSize(),
            (std::vector<char>{'a', 'b', 'c'}));
        file.setDataAndSize({'x', 'y', 'z'});
        TS_ASSERT_EQUALS(written,
            (std::vector<char>{'x', 'y', 'z'}));
    }

    void testOpenFromLocalFilenameAndRejectsNul() {
        char path[] = "/tmp/gphoto2pp-open-XXXXXX";
        const int fd = mkstemp(path);
        TS_ASSERT(fd >= 0);
        if (fd < 0) {
            return;
        }
        const char data[] = "legacy";
        TS_ASSERT_EQUALS(write(fd, data, 6), 6);
        close(fd);

        gphoto2pp::CameraFileWrapper file;
        TS_ASSERT_THROWS_NOTHING(file.open(path));
        TS_ASSERT_EQUALS(file.getDataAndSize(),
            (std::vector<char>{'l', 'e', 'g', 'a', 'c', 'y'}));
        TS_ASSERT_THROWS(file.open(
            std::string("bad\0path", 8)),
            const gphoto2pp::exceptions::ArgumentException &);
        unlink(path);
    }
};
