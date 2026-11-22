#include <cxxtest/TestSuite.h>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/gp_port_wrapper.hpp>

namespace gphoto2 {
#include <gphoto2/gphoto2-port.h>
#include <gphoto2/gphoto2-port-info-list.h>
#include <gphoto2/gphoto2-result.h>

struct _GPPortInfoList {
    int marker;
};

struct _GPPortInfo {
    char *name;
    char *path;
    GPPortType type;
};
}

#include <cstdint>
#include <limits>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

static_assert(!std::is_copy_constructible<
    gphoto2pp::GPPortWrapper>::value,
    "GPPort wrappers must not be copied");

namespace {
gphoto2::GPPort fake_port{};
gphoto2::GPPortInfo fake_port_info = nullptr;
gphoto2::_GPPortInfo fake_port_info_storage{};
gphoto2::GPPortInfoList fake_port_info_list{};
int port_new_calls = 0;
int port_free_calls = 0;
int port_open_calls = 0;
int port_close_calls = 0;
int port_reset_calls = 0;
int port_write_calls = 0;
int port_read_result = 3;
int port_interrupt_result = 2;
int port_result = GP_OK;
std::vector<char> written_data;
std::string port_error;
int usb_sys_device = -1;
int clear_halt_calls = 0;
int clear_halt_endpoint = -1;
int seek_calls = 0;
int seek_offset = 0;
int seek_whence = 0;
int usb_find_calls = 0;
int usb_find_vendor = 0;
int usb_find_product = 0;
int usb_class_calls = 0;
int usb_msg_calls = 0;
int scsi_calls = 0;
}

namespace gphoto2 {
extern "C" int gp_port_new(GPPort **port) {
    ++port_new_calls;
    *port = &fake_port;
    return GP_OK;
}

extern "C" int gp_port_free(GPPort *) {
    ++port_free_calls;
    return GP_OK;
}

extern "C" int gp_port_open(GPPort *) {
    ++port_open_calls;
    return port_result;
}

extern "C" int gp_port_close(GPPort *) {
    ++port_close_calls;
    return port_result;
}

extern "C" int gp_port_reset(GPPort *) {
    ++port_reset_calls;
    return port_result;
}

extern "C" int gp_port_write(GPPort *, const char *data, int size) {
    ++port_write_calls;
    written_data.assign(data, data + size);
    return size;
}

extern "C" int gp_port_read(GPPort *, char *data, int size) {
    if (port_read_result < 0) {
        return port_read_result;
    }
    const int actual = port_read_result > size ? size : port_read_result;
    std::memcpy(data, "abc", static_cast<std::size_t>(actual));
    return actual;
}

extern "C" int gp_port_check_int(GPPort *, char *data, int size) {
    const int actual = port_interrupt_result > size
        ? size : port_interrupt_result;
    std::memcpy(data, "xy", static_cast<std::size_t>(actual));
    return actual;
}

extern "C" int gp_port_check_int_fast(GPPort *, char *data, int size) {
    return gp_port_check_int(nullptr, data, size);
}

extern "C" int gp_port_get_timeout(GPPort *port, int *timeout) {
    *timeout = port->timeout;
    return GP_OK;
}

extern "C" int gp_port_set_timeout(GPPort *port, int timeout) {
    port->timeout = timeout;
    return GP_OK;
}

extern "C" int gp_port_get_settings(GPPort *port, GPPortSettings *settings) {
    *settings = port->settings;
    return GP_OK;
}

extern "C" int gp_port_set_settings(GPPort *port, GPPortSettings settings) {
    port->settings = settings;
    return GP_OK;
}

extern "C" int gp_port_get_pin(GPPort *, GPPin, GPLevel *level) {
    *level = GP_LEVEL_HIGH;
    return GP_OK;
}

extern "C" int gp_port_set_pin(GPPort *, GPPin, GPLevel) {
    return GP_OK;
}

extern "C" int gp_port_send_break(GPPort *, int) {
    return port_result;
}

extern "C" int gp_port_flush(GPPort *, int) {
    return port_result;
}

extern "C" int gp_port_usb_set_sys_device(int fd) {
    usb_sys_device = fd;
    return GP_OK;
}

extern "C" int gp_port_usb_get_sys_device() {
    return usb_sys_device;
}

extern "C" int gp_port_usb_clear_halt(GPPort *, int endpoint) {
    ++clear_halt_calls;
    clear_halt_endpoint = endpoint;
    return port_result;
}

extern "C" int gp_port_seek(GPPort *, int offset, int whence) {
    ++seek_calls;
    seek_offset = offset;
    seek_whence = whence;
    return GP_OK;
}

extern "C" int gp_port_usb_find_device(
    GPPort *, int vendor, int product) {
    ++usb_find_calls;
    usb_find_vendor = vendor;
    usb_find_product = product;
    return port_result;
}

extern "C" int gp_port_usb_find_device_by_class(
    GPPort *, int, int, int) {
    ++usb_class_calls;
    return port_result;
}

extern "C" int gp_port_usb_msg_write(
    GPPort *, int, int, int, char *data, int size) {
    ++usb_msg_calls;
    written_data.clear();
    if (data != nullptr && size > 0) {
        written_data.assign(data, data + size);
    }
    return size;
}

extern "C" int gp_port_usb_msg_read(
    GPPort *, int, int, int, char *data, int size) {
    ++usb_msg_calls;
    if (size > 0) {
        std::memcpy(data, "usb", size > 3 ? 3 : size);
    }
    return size > 3 ? 3 : size;
}

extern "C" int gp_port_usb_msg_interface_write(
    GPPort *, int, int, int, char *data, int size) {
    return gp_port_usb_msg_write(
        nullptr, 0, 0, 0, data, size);
}

extern "C" int gp_port_usb_msg_interface_read(
    GPPort *, int, int, int, char *data, int size) {
    return gp_port_usb_msg_read(nullptr, 0, 0, 0, data, size);
}

extern "C" int gp_port_usb_msg_class_write(
    GPPort *, int, int, int, char *data, int size) {
    return gp_port_usb_msg_write(
        nullptr, 0, 0, 0, data, size);
}

extern "C" int gp_port_usb_msg_class_read(
    GPPort *, int, int, int, char *data, int size) {
    return gp_port_usb_msg_read(nullptr, 0, 0, 0, data, size);
}

extern "C" int gp_port_send_scsi_cmd(
    GPPort *, int, char *command, int command_size,
    char *sense, int sense_size, char *data, int data_size) {
    ++scsi_calls;
    (void)command;
    (void)command_size;
    if (sense != nullptr && sense_size > 0) {
        std::memcpy(sense, "s", sense_size > 1 ? 1 : sense_size);
    }
    if (data != nullptr && data_size > 0) {
        std::memcpy(data, "d", data_size > 1 ? 1 : data_size);
    }
    return GP_OK;
}

extern "C" int gp_port_set_error(GPPort *, const char *format, ...) {
    (void)format;
    port_error = "Synthetic port error";
    return GP_OK;
}

extern "C" const char *gp_port_get_error(GPPort *) {
    return port_error.empty() ? nullptr : port_error.c_str();
}

extern "C" int gp_port_info_new(GPPortInfo *info) {
    fake_port_info = &fake_port_info_storage;
    *info = fake_port_info;
    return GP_OK;
}

extern "C" int gp_port_info_list_new(GPPortInfoList **list) {
    *list = &fake_port_info_list;
    return GP_OK;
}

extern "C" int gp_port_info_list_append(GPPortInfoList *, GPPortInfo) {
    return 0;
}

extern "C" int gp_port_info_list_free(GPPortInfoList *) {
    fake_port_info = nullptr;
    return GP_OK;
}

extern "C" int gp_port_info_set_name(GPPortInfo info, const char *name) {
    (void)name;
    info->name = const_cast<char *>("Synthetic port");
    return GP_OK;
}

extern "C" int gp_port_info_set_path(GPPortInfo info, const char *path) {
    (void)path;
    info->path = const_cast<char *>("usb:001");
    return GP_OK;
}

extern "C" int gp_port_info_set_type(GPPortInfo info, GPPortType type) {
    info->type = type;
    return GP_OK;
}

extern "C" int gp_port_get_info(GPPort *, GPPortInfo *info) {
    fake_port_info = &fake_port_info_storage;
    if (fake_port_info_storage.name == nullptr) {
        fake_port_info_storage.name = const_cast<char *>("Synthetic port");
    }
    if (fake_port_info_storage.path == nullptr) {
        fake_port_info_storage.path = const_cast<char *>("usb:001");
    }
    if (fake_port_info_storage.type == 0) {
        fake_port_info_storage.type = GP_PORT_USB;
    }
    *info = fake_port_info;
    return GP_OK;
}

extern "C" int gp_port_info_get_name(GPPortInfo info, char **name) {
    *name = info->name;
    return GP_OK;
}

extern "C" int gp_port_info_get_path(GPPortInfo info, char **path) {
    *path = info->path;
    return GP_OK;
}

extern "C" int gp_port_info_get_type(GPPortInfo info, GPPortType *type) {
    *type = info->type;
    return GP_OK;
}

extern "C" int gp_port_set_info(GPPort *, GPPortInfo) {
    return GP_OK;
}
}

class GPPortWrapper_Capabilities : public CxxTest::TestSuite {
public:
    void setUp() {
        fake_port = {};
        fake_port_info = nullptr;
        fake_port_info_list = {};
        port_new_calls = 0;
        port_free_calls = 0;
        port_open_calls = 0;
        port_close_calls = 0;
        port_reset_calls = 0;
        port_write_calls = 0;
        port_read_result = 3;
        port_interrupt_result = 2;
        port_result = GP_OK;
        written_data.clear();
        port_error.clear();
        usb_sys_device = -1;
        clear_halt_calls = 0;
        clear_halt_endpoint = -1;
        seek_calls = 0;
        seek_offset = 0;
        seek_whence = 0;
        usb_find_calls = 0;
        usb_find_vendor = 0;
        usb_find_product = 0;
        usb_class_calls = 0;
        usb_msg_calls = 0;
        scsi_calls = 0;
    }

    void testLifecycleIoAndTimeout() {
        gphoto2pp::GPPortWrapper port;
        TS_ASSERT_EQUALS(port_new_calls, 1);
        port.open();
        port.close();
        port.reset();
        TS_ASSERT_EQUALS(port_open_calls, 1);
        TS_ASSERT_EQUALS(port_close_calls, 1);
        TS_ASSERT_EQUALS(port_reset_calls, 1);

        port.setTimeout(250);
        TS_ASSERT_EQUALS(port.getTimeout(), 250);
        const std::vector<char> data = {'a', '\0', 'b'};
        TS_ASSERT_EQUALS(port.write(data), 3);
        TS_ASSERT_EQUALS(written_data, data);
        TS_ASSERT_EQUALS(port.read(4).size(), 3U);
        TS_ASSERT_EQUALS(port.checkInterrupt(4).size(), 2U);
        TS_ASSERT_EQUALS(port.checkInterruptFast(4).size(), 2U);
    }

    void testPortInfoAndSerialSettings() {
        gphoto2pp::GPPortWrapper port;
        gphoto2pp::PortInfoSnapshot info;
        info.name = "Synthetic port";
        info.path = "usb:001";
        info.type = gphoto2pp::GPPortTypeWrapper::USB;
        port.setInfo(info);
        const auto retrieved = port.getInfo();
        TS_ASSERT_EQUALS(retrieved.name, "Synthetic port");
        TS_ASSERT_EQUALS(retrieved.path, "usb:001");

        gphoto2pp::SerialPortSettings serial;
        serial.port = "/dev/test";
        serial.speed = 115200;
        serial.bits = 8;
        serial.parity = gphoto2pp::PortSerialParity::Even;
        serial.stopBits = 1;
        port.setSerialSettings(serial);
        const auto readback = port.getSerialSettings();
        TS_ASSERT_EQUALS(readback.port, "/dev/test");
        TS_ASSERT_EQUALS(readback.speed, 115200);
        TS_ASSERT_EQUALS(readback.parity,
            gphoto2pp::PortSerialParity::Even);
    }

    void testUsbSettingsPinsBreakAndFlush() {
        gphoto2pp::GPPortWrapper port;
        gphoto2pp::USBPortSettings usb;
        usb.port = "usb:001";
        usb.inEndpoint = 0x81;
        usb.outEndpoint = 0x02;
        usb.interruptEndpoint = 0x83;
        port.setUSBSettings(usb);
        const auto readback = port.getUSBSettings();
        TS_ASSERT_EQUALS(readback.port, "usb:001");
        TS_ASSERT_EQUALS(readback.inEndpoint, 0x81);
        TS_ASSERT_EQUALS(readback.outEndpoint, 0x02);

        port.setPin(gphoto2pp::PortPin::RTS,
            gphoto2pp::PortPinLevel::High);
        TS_ASSERT_EQUALS(port.getPin(gphoto2pp::PortPin::RTS),
            gphoto2pp::PortPinLevel::High);
        port.sendBreak(50);
        port.flush(gphoto2pp::PortFlushDirection::Both);
    }

    void testUsbSystemDeviceClearHaltAndSeek() {
        gphoto2pp::GPPortWrapper::setUSBSystemDevice(42);
        TS_ASSERT_EQUALS(
            gphoto2pp::GPPortWrapper::getUSBSystemDevice(), 42);
        gphoto2pp::GPPortWrapper port;
        port.clearUSBHalt(0x81);
        TS_ASSERT_EQUALS(clear_halt_calls, 1);
        TS_ASSERT_EQUALS(clear_halt_endpoint, 0x81);
        port.seek(123, gphoto2pp::PortSeekWhence::Current);
        TS_ASSERT_EQUALS(seek_calls, 1);
        TS_ASSERT_EQUALS(seek_offset, 123);
        TS_ASSERT_EQUALS(seek_whence, 1);
    }

    void testUsbDiscoveryAndControlTransfers() {
        gphoto2pp::GPPortWrapper port;
        port.findUSBDevice(0x1234, 0x5678);
        TS_ASSERT_EQUALS(usb_find_calls, 1);
        TS_ASSERT_EQUALS(usb_find_vendor, 0x1234);
        TS_ASSERT_EQUALS(usb_find_product, 0x5678);
        port.findUSBDeviceByClass(6, 1, 1);
        TS_ASSERT_EQUALS(usb_class_calls, 1);

        const std::vector<char> payload = {'x', 'y'};
        TS_ASSERT_EQUALS(
            port.writeUSBMessage(0x40, 1, 0, payload), 2);
        TS_ASSERT_EQUALS(written_data, payload);
        TS_ASSERT_EQUALS(port.readUSBMessage(
            0xc0, 1, 0, 5).size(), 3U);

        port.writeUSBInterfaceMessage(0x41, 1, 0, payload);
        TS_ASSERT_EQUALS(port.readUSBInterfaceMessage(
            0xc1, 1, 0, 5).size(), 3U);
        port.writeUSBClassMessage(0x21, 1, 0, payload);
        TS_ASSERT_EQUALS(port.readUSBClassMessage(
            0xa1, 1, 0, 5).size(), 3U);
    }

    void testScsiCommand() {
        gphoto2pp::GPPortWrapper port;
        std::vector<char> command = {0x12, 0x00};
        std::vector<char> sense(1);
        std::vector<char> data(1);
        TS_ASSERT_EQUALS(
            port.sendSCSICommand(true, command, sense, data), GP_OK);
        TS_ASSERT_EQUALS(scsi_calls, 1);
        TS_ASSERT_EQUALS(sense[0], 's');
        TS_ASSERT_EQUALS(data[0], 'd');
    }

    void testPortErrorState() {
        gphoto2pp::GPPortWrapper port;
        port.setError("Synthetic port error");
        TS_ASSERT_EQUALS(port.getError(), "Synthetic port error");
        TS_ASSERT_THROWS(port.setError(
            std::string("bad\0error", 9)),
            const gphoto2pp::exceptions::ArgumentException &);
    }

    void testInvalidInputsAndMovedFromPort() {
        gphoto2pp::GPPortWrapper port;
        TS_ASSERT_THROWS(port.setTimeout(-1),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(port.sendBreak(-1),
            const gphoto2pp::exceptions::ArgumentException &);
        TS_ASSERT_THROWS(port.read(
            static_cast<std::size_t>(std::numeric_limits<int>::max()) + 1U),
            const gphoto2pp::exceptions::ArgumentException &);

        gphoto2pp::GPPortWrapper moved(std::move(port));
        TS_ASSERT(port.getPtr() == nullptr);
        TS_ASSERT_THROWS(port.open(), const std::logic_error &);
    }
};
