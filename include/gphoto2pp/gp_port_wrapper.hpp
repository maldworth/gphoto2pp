/** \file
 * \brief Owned wrapper for libgphoto2_port GPPort operations.
 */
#ifndef GPHOTO2PP_GP_PORT_WRAPPER_HPP
#define GPHOTO2PP_GP_PORT_WRAPPER_HPP

#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace gphoto2
{
	struct _GPPort;
}

namespace gphoto2pp
{
	enum class PortSerialParity : int
	{
		Off = 0,
		Even = 1,
		Odd = 2
	};

	enum class PortPin : int
	{
		RTS = 0,
		DTR = 1,
		CTS = 2,
		DSR = 3,
		CD = 4,
		Ring = 5
	};

	enum class PortPinLevel : int
	{
		Low = 0,
		High = 1
	};

	enum class PortFlushDirection : int
	{
		Input = 0,
		Output = 1,
		Both = 2
	};

	enum class PortSeekWhence : int
	{
		Set = 0,
		Current = 1,
		End = 2
	};

	struct SerialPortSettings
	{
		std::string port;
		int speed = 0;
		int bits = 8;
		PortSerialParity parity = PortSerialParity::Off;
		int stopBits = 1;
	};

	struct USBPortSettings
	{
		int inEndpoint = -1;
		int outEndpoint = -1;
		int interruptEndpoint = -1;
		int config = -1;
		int interface = 0;
		int alternateSetting = -1;
		int maxPacketSize = 0;
		std::string port;
	};

	/**
	 * \brief Move-only owner of a libgphoto2_port GPPort.
	 *
	 * Read and write operations do not open or close the port; callers must
	 * use open() and close() around I/O that requires an initialized port.
	 */
	class GPPortWrapper
	{
	public:
		GPPortWrapper();
		~GPPortWrapper();
		GPPortWrapper(GPPortWrapper&& other) noexcept;
		GPPortWrapper& operator=(GPPortWrapper&& other);
		GPPortWrapper(GPPortWrapper const &) = delete;
		GPPortWrapper& operator=(GPPortWrapper const &) = delete;

		gphoto2::_GPPort* getPtr() const;

		void setInfo(PortInfoSnapshot const & info);
		PortInfoSnapshot getInfo() const;

		void open();
		void close();
		void reset();

		int write(std::vector<char> const & data);
		std::vector<char> read(std::size_t size);
		std::vector<char> checkInterrupt(std::size_t size);
		std::vector<char> checkInterruptFast(std::size_t size);

		int getTimeout() const;
		void setTimeout(int timeout);

		SerialPortSettings getSerialSettings() const;
		void setSerialSettings(SerialPortSettings const & settings);
		USBPortSettings getUSBSettings() const;
		void setUSBSettings(USBPortSettings const & settings);

		PortPinLevel getPin(PortPin pin) const;
		void setPin(PortPin pin, PortPinLevel level);
		void sendBreak(int duration);
		void flush(PortFlushDirection direction);
		void setError(std::string const & text);
		std::string getError() const;

		static void setUSBSystemDevice(int fd);
		static int getUSBSystemDevice();
		void clearUSBHalt(int endpoint);
		void seek(int offset, PortSeekWhence whence);
		void findUSBDevice(int vendorId, int productId);
		void findUSBDeviceByClass(int mainClass, int subClass,
			int protocol);
		int writeUSBMessage(int request, int value, int index,
			std::vector<char> const & data);
		std::vector<char> readUSBMessage(int request, int value, int index,
			std::size_t size);
		int writeUSBInterfaceMessage(int request, int value, int index,
			std::vector<char> const & data);
		std::vector<char> readUSBInterfaceMessage(int request, int value,
			int index, std::size_t size);
		int writeUSBClassMessage(int request, int value, int index,
			std::vector<char> const & data);
		std::vector<char> readUSBClassMessage(int request, int value,
			int index, std::size_t size);
		int sendSCSICommand(bool toDevice,
			std::vector<char> const & command,
			std::vector<char> & sense, std::vector<char> & data);

	private:
		gphoto2::_GPPort* requirePort() const;
		gphoto2::_GPPort* m_port;
	};
}

#endif // GPHOTO2PP_GP_PORT_WRAPPER_HPP
