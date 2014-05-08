/** \file 
 * \author Copyright (c) 2013 maldworth <https://github.com/maldworth>
 *
 * \note
 * This file is part of gphoto2pp
 * 
 * \note
 * gphoto2pp is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * \note
 * gphoto2pp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * \note
 * You should have received a copy of the GNU Lesser General Public
 * License along with gphoto2pp.
 * If not, see http://www.gnu.org/licenses
 */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>

namespace gphoto2pp
{
	namespace exceptions
	{
		/**
		 * \class GPhoto2ppException
		 * Base class for all exceptions in this library
		 */
		class GPhoto2ppException : public std::runtime_error
		{
		public:
			GPhoto2ppException(std::string&& message) : std::runtime_error(message) { }
		};
		
		/**
		 * \class gphoto2_exception
		 * Represents any error received from a gphoto2 method call with a value < 0
		 */
		class gphoto2_exception : public GPhoto2ppException
		{
		public:
			gphoto2_exception(int result, std::string&& gp_result_string) : GPhoto2ppException(std::move(gp_result_string)),m_resultCode(result) { }
			
			/**
			 * \brief The error code received from the gphoto2 method
			 */
			int getResultCode() const
			{
				return m_resultCode;
			}
		private:
			const int m_resultCode;
			
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
