#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>

namespace gphoto2pp
{
	namespace exceptions
	{
		/**
		 * \class gphoto2_exception
		 * Represents any error received from a gphoto2 method call with a value < 0
		 */
		class gphoto2_exception : public std::runtime_error
		{
		public:
			gphoto2_exception(int result, std::string&& gp_result_string) : std::runtime_error(gp_result_string),m_resultCode(result) { }
			
			/**
			 * \brief The error code received from the gphoto2 method
			 */
			int getResultCode()
			{
				return m_resultCode;
			}
		private:
			const int m_resultCode;
			
		};
		
		class GPhoto2ppException : public std::runtime_error
		{
		public:
			GPhoto2ppException(std::string&& message) : std::runtime_error(message) { }
		};
		
		class InvalidLinkedVersionException : public GPhoto2ppException
		{
		public:
			InvalidLinkedVersionException(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		class CameraWrapperException : public GPhoto2ppException
		{
		public:
			CameraWrapperException(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		class HelperException : public GPhoto2ppException
		{
		public:
			HelperException(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		class NoCameraFoundError : public GPhoto2ppException
		{
		public:
			NoCameraFoundError(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		class ArgumentException : public GPhoto2ppException
		{
		public:
			ArgumentException(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		// Widget Exceptions
		class InvalidWidgetType : public GPhoto2ppException
		{
		public:
			InvalidWidgetType(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		class NullWidget : public GPhoto2ppException
		{
		public:
			NullWidget(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		class IndexOutOfRange : public GPhoto2ppException
		{
		public:
			IndexOutOfRange(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};
		
		class ValueOutOfLimits : public GPhoto2ppException
		{
		public:
			ValueOutOfLimits(std::string&& message) : GPhoto2ppException(std::move(message)) { }
		};	
		
			
	}
}

#endif // EXCEPTIONS_HPP
