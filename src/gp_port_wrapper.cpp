/** \file
 * \brief Implementation of GPPortWrapper.
 */

#include <gphoto2pp/gp_port_wrapper.hpp>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/helper_gphoto2.hpp>

#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace gphoto2
{
#include <gphoto2/gphoto2-port.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

namespace gphoto2pp
{
	static_assert(static_cast<int>(PortSerialParity::Off) ==
		gphoto2::GP_PORT_SERIAL_PARITY_OFF,
		"Serial parity constants must match libgphoto2_port");
	static_assert(static_cast<int>(PortSerialParity::Even) ==
		gphoto2::GP_PORT_SERIAL_PARITY_EVEN &&
		static_cast<int>(PortSerialParity::Odd) ==
		gphoto2::GP_PORT_SERIAL_PARITY_ODD,
		"Serial parity constants must match libgphoto2_port");
	static_assert(static_cast<int>(PortPin::RTS) == gphoto2::GP_PIN_RTS &&
		static_cast<int>(PortPin::DTR) == gphoto2::GP_PIN_DTR &&
		static_cast<int>(PortPin::CTS) == gphoto2::GP_PIN_CTS &&
		static_cast<int>(PortPin::DSR) == gphoto2::GP_PIN_DSR &&
		static_cast<int>(PortPin::CD) == gphoto2::GP_PIN_CD &&
		static_cast<int>(PortPin::Ring) == gphoto2::GP_PIN_RING,
		"Port pin constants must match libgphoto2_port");
	static_assert(static_cast<int>(PortPinLevel::Low) ==
		gphoto2::GP_LEVEL_LOW &&
		static_cast<int>(PortPinLevel::High) ==
		gphoto2::GP_LEVEL_HIGH,
		"Port pin level constants must match libgphoto2_port");

	namespace
	{
		std::string fixedPortString(char const *value, std::size_t size,
			char const *fieldName)
		{
			if (value == nullptr)
			{
				throw std::runtime_error(
					std::string("libgphoto2_port returned null ") + fieldName);
			}
			auto *end = static_cast<char const *>(
				std::memchr(value, '\0', size));
			if (end == nullptr)
			{
				throw std::runtime_error(
					std::string("libgphoto2_port returned unterminated ") +
					fieldName);
			}
			return std::string(value, static_cast<std::size_t>(end - value));
		}
	}

	GPPortWrapper::GPPortWrapper()
		: m_port(nullptr)
	{
		gphoto2::_GPPort *rawPort = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_port_new(&rawPort),
			"gp_port_new");
		if (rawPort == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2_port returned a null GPPort");
		}
		m_port = rawPort;
	}

	GPPortWrapper::~GPPortWrapper()
	{
		if (m_port != nullptr)
		{
			gphoto2pp::checkResponseSilent(
				gphoto2::gp_port_free(m_port), "gp_port_free");
		}
	}

	GPPortWrapper::GPPortWrapper(GPPortWrapper&& other) noexcept
		: m_port(other.m_port)
	{
		other.m_port = nullptr;
	}

	GPPortWrapper& GPPortWrapper::operator=(GPPortWrapper&& other)
	{
		if (this != &other)
		{
			if (m_port != nullptr)
			{
				gphoto2pp::checkResponse(
					gphoto2::gp_port_free(m_port), "gp_port_free");
			}
			m_port = other.m_port;
			other.m_port = nullptr;
		}
		return *this;
	}

	gphoto2::_GPPort* GPPortWrapper::getPtr() const
	{
		return m_port;
	}

	gphoto2::_GPPort* GPPortWrapper::requirePort() const
	{
		if (m_port == nullptr)
		{
			throw std::logic_error("GPPort wrapper has been moved from");
		}
		return m_port;
	}

	void GPPortWrapper::setInfo(PortInfoSnapshot const & info)
	{
		if (info.name.find('\0') != std::string::npos ||
			info.path.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Port information strings must not contain embedded NUL");
		}
		gphoto2::GPPortInfo portInfo = nullptr;
		gphoto2::GPPortInfoList *list = nullptr;
		try
		{
			gphoto2pp::checkResponse(gphoto2::gp_port_info_list_new(&list),
				"gp_port_info_list_new");
			gphoto2pp::checkResponse(gphoto2::gp_port_info_new(&portInfo),
				"gp_port_info_new");
			gphoto2pp::checkResponse(gphoto2::gp_port_info_list_append(
				list, portInfo),
				"gp_port_info_list_append");
			gphoto2pp::checkResponse(gphoto2::gp_port_info_set_name(
				portInfo, info.name.c_str()), "gp_port_info_set_name");
			gphoto2pp::checkResponse(gphoto2::gp_port_info_set_path(
				portInfo, info.path.c_str()), "gp_port_info_set_path");
			gphoto2pp::checkResponse(gphoto2::gp_port_info_set_type(
				portInfo, static_cast<gphoto2::GPPortType>(info.type)),
				"gp_port_info_set_type");
			gphoto2pp::checkResponse(gphoto2::gp_port_set_info(
				this->requirePort(), portInfo), "gp_port_set_info");
		}
		catch (...)
		{
			if (list != nullptr)
			{
				gphoto2::gp_port_info_list_free(list);
			}
			throw;
		}
		gphoto2pp::checkResponseSilent(
			gphoto2::gp_port_info_list_free(list),
			"gp_port_info_list_free");
	}

	PortInfoSnapshot GPPortWrapper::getInfo() const
	{
		gphoto2::GPPortInfo portInfo = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_port_get_info(
			this->requirePort(), &portInfo), "gp_port_get_info");
		if (portInfo == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2_port returned a null port information entry");
		}
		char *name = nullptr;
		char *path = nullptr;
		gphoto2::GPPortType type = gphoto2::GP_PORT_NONE;
		gphoto2pp::checkResponse(gphoto2::gp_port_info_get_name(
			portInfo, &name), "gp_port_info_get_name");
		gphoto2pp::checkResponse(gphoto2::gp_port_info_get_path(
			portInfo, &path), "gp_port_info_get_path");
		gphoto2pp::checkResponse(gphoto2::gp_port_info_get_type(
			portInfo, &type), "gp_port_info_get_type");
		if (name == nullptr || path == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2_port returned incomplete port information");
		}
		PortInfoSnapshot snapshot;
		snapshot.name = name;
		snapshot.path = path;
		snapshot.type = static_cast<GPPortTypeWrapper>(type);
		return snapshot;
	}

	void GPPortWrapper::open()
	{
		gphoto2pp::checkResponse(gphoto2::gp_port_open(this->requirePort()),
			"gp_port_open");
	}

	void GPPortWrapper::close()
	{
		gphoto2pp::checkResponse(gphoto2::gp_port_close(this->requirePort()),
			"gp_port_close");
	}

	void GPPortWrapper::reset()
	{
		gphoto2pp::checkResponse(gphoto2::gp_port_reset(this->requirePort()),
			"gp_port_reset");
	}

	int GPPortWrapper::write(std::vector<char> const & data)
	{
		if (data.size() >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"GPPort write size exceeds libgphoto2_port int limit");
		}
		if (data.empty())
		{
			return 0;
		}
		const int result = gphoto2::gp_port_write(
			this->requirePort(), data.data(),
			static_cast<int>(data.size()));
		gphoto2pp::checkResponse(result, "gp_port_write");
		return result;
	}

	std::vector<char> GPPortWrapper::read(std::size_t size)
	{
		if (size > static_cast<std::size_t>(
			(std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"GPPort read size exceeds libgphoto2_port int limit");
		}
		if (size == 0)
		{
			return {};
		}
		std::vector<char> data(size);
		const int result = gphoto2::gp_port_read(
			this->requirePort(), data.data(), static_cast<int>(size));
		gphoto2pp::checkResponse(result, "gp_port_read");
		if (result < 0 || static_cast<std::size_t>(result) > size)
		{
			throw std::runtime_error(
				"libgphoto2_port reported an invalid read length");
		}
		data.resize(static_cast<std::size_t>(result));
		return data;
	}

	std::vector<char> GPPortWrapper::checkInterrupt(std::size_t size)
	{
		if (size > static_cast<std::size_t>(
			(std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"GPPort interrupt size exceeds libgphoto2_port int limit");
		}
		if (size == 0)
		{
			return {};
		}
		std::vector<char> data(size);
		const int result = gphoto2::gp_port_check_int(
			this->requirePort(), data.data(), static_cast<int>(size));
		gphoto2pp::checkResponse(result, "gp_port_check_int");
		if (static_cast<std::size_t>(result) > size)
		{
			throw std::runtime_error(
				"libgphoto2_port reported an invalid interrupt read length");
		}
		data.resize(static_cast<std::size_t>(result));
		return data;
	}

	std::vector<char> GPPortWrapper::checkInterruptFast(std::size_t size)
	{
		if (size > static_cast<std::size_t>(
			(std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"GPPort interrupt size exceeds libgphoto2_port int limit");
		}
		if (size == 0)
		{
			return {};
		}
		std::vector<char> data(size);
		const int result = gphoto2::gp_port_check_int_fast(
			this->requirePort(), data.data(), static_cast<int>(size));
		gphoto2pp::checkResponse(result, "gp_port_check_int_fast");
		if (static_cast<std::size_t>(result) > size)
		{
			throw std::runtime_error(
				"libgphoto2_port reported an invalid fast interrupt read length");
		}
		data.resize(static_cast<std::size_t>(result));
		return data;
	}

	int GPPortWrapper::getTimeout() const
	{
		int timeout = 0;
		gphoto2pp::checkResponse(gphoto2::gp_port_get_timeout(
			this->requirePort(), &timeout), "gp_port_get_timeout");
		return timeout;
	}

	void GPPortWrapper::setTimeout(int timeout)
	{
		if (timeout < 0)
		{
			throw exceptions::ArgumentException(
				"GPPort timeout must be nonnegative");
		}
		gphoto2pp::checkResponse(gphoto2::gp_port_set_timeout(
			this->requirePort(), timeout), "gp_port_set_timeout");
	}

	SerialPortSettings GPPortWrapper::getSerialSettings() const
	{
		gphoto2::GPPortSettings settings{};
		gphoto2pp::checkResponse(gphoto2::gp_port_get_settings(
			this->requirePort(), &settings), "gp_port_get_settings");
		SerialPortSettings result;
		result.port = fixedPortString(settings.serial.port,
			sizeof(settings.serial.port), "serial port setting");
		result.speed = settings.serial.speed;
		result.bits = settings.serial.bits;
		result.parity = static_cast<PortSerialParity>(
			settings.serial.parity);
		result.stopBits = settings.serial.stopbits;
		return result;
	}

	void GPPortWrapper::setSerialSettings(
		SerialPortSettings const & settings)
	{
		if (settings.port.find('\0') != std::string::npos ||
			settings.port.size() >= 128)
		{
			throw exceptions::ArgumentException(
				"Serial port name is invalid or exceeds libgphoto2_port limit");
		}
		gphoto2::GPPortSettings value{};
		std::memcpy(value.serial.port, settings.port.c_str(),
			settings.port.size());
		value.serial.speed = settings.speed;
		value.serial.bits = settings.bits;
		value.serial.parity =
			static_cast<gphoto2::GPPortSerialParity>(settings.parity);
		value.serial.stopbits = settings.stopBits;
		gphoto2pp::checkResponse(gphoto2::gp_port_set_settings(
			this->requirePort(), value), "gp_port_set_settings");
	}

	USBPortSettings GPPortWrapper::getUSBSettings() const
	{
		gphoto2::GPPortSettings settings{};
		gphoto2pp::checkResponse(gphoto2::gp_port_get_settings(
			this->requirePort(), &settings), "gp_port_get_settings");
		USBPortSettings result;
		result.inEndpoint = settings.usb.inep;
		result.outEndpoint = settings.usb.outep;
		result.interruptEndpoint = settings.usb.intep;
		result.config = settings.usb.config;
		result.interface = settings.usb.interface;
		result.alternateSetting = settings.usb.altsetting;
		result.maxPacketSize = settings.usb.maxpacketsize;
		result.port = fixedPortString(settings.usb.port,
			sizeof(settings.usb.port), "USB port setting");
		return result;
	}

	void GPPortWrapper::setUSBSettings(USBPortSettings const & settings)
	{
		if (settings.port.find('\0') != std::string::npos ||
			settings.port.size() >= 64)
		{
			throw exceptions::ArgumentException(
				"USB port name is invalid or exceeds libgphoto2_port limit");
		}
		gphoto2::GPPortSettings value{};
		value.usb.inep = settings.inEndpoint;
		value.usb.outep = settings.outEndpoint;
		value.usb.intep = settings.interruptEndpoint;
		value.usb.config = settings.config;
		value.usb.interface = settings.interface;
		value.usb.altsetting = settings.alternateSetting;
		value.usb.maxpacketsize = settings.maxPacketSize;
		std::memcpy(value.usb.port, settings.port.c_str(),
			settings.port.size());
		gphoto2pp::checkResponse(gphoto2::gp_port_set_settings(
			this->requirePort(), value), "gp_port_set_settings");
	}

	PortPinLevel GPPortWrapper::getPin(PortPin pin) const
	{
		gphoto2::GPLevel level = gphoto2::GP_LEVEL_LOW;
		gphoto2pp::checkResponse(gphoto2::gp_port_get_pin(
			this->requirePort(), static_cast<gphoto2::GPPin>(pin), &level),
			"gp_port_get_pin");
		return static_cast<PortPinLevel>(level);
	}

	void GPPortWrapper::setPin(PortPin pin, PortPinLevel level)
	{
		gphoto2pp::checkResponse(gphoto2::gp_port_set_pin(
			this->requirePort(), static_cast<gphoto2::GPPin>(pin),
			static_cast<gphoto2::GPLevel>(level)), "gp_port_set_pin");
	}

	void GPPortWrapper::sendBreak(int duration)
	{
		if (duration < 0)
		{
			throw exceptions::ArgumentException(
				"GPPort break duration must be nonnegative");
		}
		gphoto2pp::checkResponse(gphoto2::gp_port_send_break(
			this->requirePort(), duration), "gp_port_send_break");
	}

	void GPPortWrapper::flush(PortFlushDirection direction)
	{
		const int value = static_cast<int>(direction);
		if (value < 0 || value > 2)
		{
			throw exceptions::ArgumentException(
				"GPPort flush direction is invalid");
		}
		gphoto2pp::checkResponse(gphoto2::gp_port_flush(
			this->requirePort(), value), "gp_port_flush");
	}

	void GPPortWrapper::setError(std::string const & text)
	{
		if (text.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"GPPort error text must not contain embedded NUL");
		}
		gphoto2pp::checkResponse(gphoto2::gp_port_set_error(
			this->requirePort(), "%s", text.c_str()), "gp_port_set_error");
	}

	std::string GPPortWrapper::getError() const
	{
		const char *error = gphoto2::gp_port_get_error(this->requirePort());
		return error == nullptr ? std::string{} : std::string(error);
	}

	void GPPortWrapper::setUSBSystemDevice(int fd)
	{
		if (fd < -1)
		{
			throw exceptions::ArgumentException(
				"USB system device descriptor must be -1 or nonnegative");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_port_usb_set_sys_device(fd),
			"gp_port_usb_set_sys_device");
	}

	int GPPortWrapper::getUSBSystemDevice()
	{
		return gphoto2::gp_port_usb_get_sys_device();
	}

	void GPPortWrapper::clearUSBHalt(int endpoint)
	{
		if (endpoint < 0)
		{
			throw exceptions::ArgumentException(
				"USB endpoint number must be nonnegative");
		}
		gphoto2pp::checkResponse(gphoto2::gp_port_usb_clear_halt(
			this->requirePort(), endpoint), "gp_port_usb_clear_halt");
	}

	void GPPortWrapper::seek(int offset, PortSeekWhence whence)
	{
		const int value = static_cast<int>(whence);
		if (value < 0 || value > 2)
		{
			throw exceptions::ArgumentException(
				"GPPort seek origin is invalid");
		}
		gphoto2pp::checkResponse(gphoto2::gp_port_seek(
			this->requirePort(), offset, value), "gp_port_seek");
	}

	void GPPortWrapper::findUSBDevice(int vendorId, int productId)
	{
		gphoto2pp::checkResponse(gphoto2::gp_port_usb_find_device(
			this->requirePort(), vendorId, productId),
			"gp_port_usb_find_device");
	}

	void GPPortWrapper::findUSBDeviceByClass(int mainClass, int subClass,
		int protocol)
	{
		gphoto2pp::checkResponse(gphoto2::gp_port_usb_find_device_by_class(
			this->requirePort(), mainClass, subClass, protocol),
			"gp_port_usb_find_device_by_class");
	}

	int GPPortWrapper::writeUSBMessage(int request, int value, int index,
		std::vector<char> const & data)
	{
		if (data.size() >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"USB message size exceeds libgphoto2_port int limit");
		}
		const int result = gphoto2::gp_port_usb_msg_write(
			this->requirePort(), request, value, index,
			const_cast<char *>(data.empty() ? nullptr : data.data()),
			static_cast<int>(data.size()));
		gphoto2pp::checkResponse(result, "gp_port_usb_msg_write");
		return result;
	}

	std::vector<char> GPPortWrapper::readUSBMessage(int request, int value,
		int index, std::size_t size)
	{
		if (size >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"USB message size exceeds libgphoto2_port int limit");
		}
		std::vector<char> data(size);
		const int result = gphoto2::gp_port_usb_msg_read(
			this->requirePort(), request, value, index,
			data.empty() ? nullptr : data.data(),
			static_cast<int>(size));
		gphoto2pp::checkResponse(result, "gp_port_usb_msg_read");
		if (static_cast<std::size_t>(result) > size)
		{
			throw std::runtime_error(
				"libgphoto2_port reported an invalid USB read length");
		}
		data.resize(static_cast<std::size_t>(result));
		return data;
	}

	int GPPortWrapper::writeUSBInterfaceMessage(int request, int value,
		int index, std::vector<char> const & data)
	{
		if (data.size() >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"USB interface message size exceeds libgphoto2_port int limit");
		}
		const int result = gphoto2::gp_port_usb_msg_interface_write(
			this->requirePort(), request, value, index,
			const_cast<char *>(data.empty() ? nullptr : data.data()),
			static_cast<int>(data.size()));
		gphoto2pp::checkResponse(
			result, "gp_port_usb_msg_interface_write");
		return result;
	}

	std::vector<char> GPPortWrapper::readUSBInterfaceMessage(int request,
		int value, int index, std::size_t size)
	{
		if (size >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"USB interface message size exceeds libgphoto2_port int limit");
		}
		std::vector<char> data(size);
		const int result = gphoto2::gp_port_usb_msg_interface_read(
			this->requirePort(), request, value, index,
			data.empty() ? nullptr : data.data(),
			static_cast<int>(size));
		gphoto2pp::checkResponse(
			result, "gp_port_usb_msg_interface_read");
		if (static_cast<std::size_t>(result) > size)
		{
			throw std::runtime_error(
				"libgphoto2_port reported an invalid USB interface read");
		}
		data.resize(static_cast<std::size_t>(result));
		return data;
	}

	int GPPortWrapper::writeUSBClassMessage(int request, int value,
		int index, std::vector<char> const & data)
	{
		if (data.size() >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"USB class message size exceeds libgphoto2_port int limit");
		}
		const int result = gphoto2::gp_port_usb_msg_class_write(
			this->requirePort(), request, value, index,
			const_cast<char *>(data.empty() ? nullptr : data.data()),
			static_cast<int>(data.size()));
		gphoto2pp::checkResponse(result, "gp_port_usb_msg_class_write");
		return result;
	}

	std::vector<char> GPPortWrapper::readUSBClassMessage(int request,
		int value, int index, std::size_t size)
	{
		if (size >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"USB class message size exceeds libgphoto2_port int limit");
		}
		std::vector<char> data(size);
		const int result = gphoto2::gp_port_usb_msg_class_read(
			this->requirePort(), request, value, index,
			data.empty() ? nullptr : data.data(),
			static_cast<int>(size));
		gphoto2pp::checkResponse(result, "gp_port_usb_msg_class_read");
		if (static_cast<std::size_t>(result) > size)
		{
			throw std::runtime_error(
				"libgphoto2_port reported an invalid USB class read");
		}
		data.resize(static_cast<std::size_t>(result));
		return data;
	}

	int GPPortWrapper::sendSCSICommand(bool toDevice,
		std::vector<char> const & command,
		std::vector<char> & sense, std::vector<char> & data)
	{
		if (command.size() >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()) ||
			sense.size() >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()) ||
			data.size() >
			static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			throw exceptions::ArgumentException(
				"GPPort SCSI buffer size exceeds libgphoto2_port int limit");
		}
		std::vector<char> commandCopy = command;
		const int result = gphoto2::gp_port_send_scsi_cmd(
			this->requirePort(), toDevice ? 1 : 0,
			commandCopy.data(),
			static_cast<int>(commandCopy.size()),
			sense.empty() ? nullptr : sense.data(),
			static_cast<int>(sense.size()),
			data.empty() ? nullptr : data.data(),
			static_cast<int>(data.size()));
		gphoto2pp::checkResponse(result, "gp_port_send_scsi_cmd");
		return result;
	}
}
