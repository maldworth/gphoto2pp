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

#include <gphoto2pp/helper_debugging.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2.h>
}

namespace gphoto2pp
{
	namespace helper
	{
		namespace debugging
		{
			namespace detail
			{
				static PortLoggingEventsManager PortLogEventHandler;
				
				// From Here http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
				inline std::string strformat(char const * fmt, va_list vl){
					int size = 512;
					char* buffer = 0;
					buffer = new char[size];
					//va_list vl;
					//va_start(vl, fmt);
					int nsize = vsnprintf(buffer, size, fmt, vl);
					if(size<=nsize){ //fail delete buffer and try again
						delete[] buffer;
						buffer = 0;
						buffer = new char[nsize+1]; //+1 for /0
						nsize = vsnprintf(buffer, size, fmt, vl);
					}
					std::string ret(buffer);
					va_end(vl);
					delete[] buffer;
					return ret;
				}
				
				inline std::string strformat(char const * fmt, ...){
					va_list vl;
					va_start(vl, fmt);
					return strformat(fmt, vl);
				}
				
#ifdef GPHOTO_LESS_25
				static void errordumper(gphoto2::GPLogLevel level, const char *domain, const char *format, va_list args, void *data)
				{
					std::string str = strformat(format, args);
#else
				static void errordumper(gphoto2::GPLogLevel level, const char *domain, const char *str, void *data)
				{
#endif
				
					switch(level)
					{
						case gphoto2::GP_LOG_ERROR:
						{
							PortLogEventHandler(LogLevelWrapper::Error, static_cast<LogLevelWrapper>(level), std::string(domain), std::string(str), data);
						}
						case gphoto2::GP_LOG_VERBOSE:
						{
							PortLogEventHandler(LogLevelWrapper::Verbose, static_cast<LogLevelWrapper>(level), std::string(domain), std::string(str), data);
						}
						case gphoto2::GP_LOG_DEBUG:
						{
							PortLogEventHandler(LogLevelWrapper::Debug, static_cast<LogLevelWrapper>(level), std::string(domain), std::string(str), data);
						}
						case gphoto2::GP_LOG_DATA:
						{
							PortLogEventHandler(LogLevelWrapper::Data, static_cast<LogLevelWrapper>(level), std::string(domain), std::string(str), data);
						}
						default:
						{
							break;
						}
					}
					
					//~ // Not sure if this is necessary, as they can log their own messages...
					//~ auto now = std::chrono::high_resolution_clock::now();
					//~ auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
					//~ auto s = std::chrono::duration_cast<std::chrono::seconds>(us);
					//~ FILE_LOG(logERROR) << (s.count() % 60) << "." << (us.count() % 1000000) << ": " << str;
				}
				
				PortLoggingEventsManager::~PortLoggingEventsManager()
				{
					stopPortLogging();
				}
				
				void PortLoggingEventsManager::startPortLogging(LogLevelWrapper const & level)
				{
					if(id == 0)
					{
						id = checkResponse(gphoto2::gp_log_add_func(static_cast<gphoto2::GPLogLevel>(level), errordumper, nullptr), "gp_log_add_func");
					}
				}
				
				void PortLoggingEventsManager::stopPortLogging()
				{
					if(id != 0)
					{
						checkResponse(gphoto2::gp_log_remove_func(id), "gp_log_remove_func");
						id = 0;
					}
				}
			}
			
			void startPortLogging(LogLevelWrapper const & level)
			{
				detail::PortLogEventHandler.startPortLogging(level);
			}
			
			void stopPortLogging()
			{
				detail::PortLogEventHandler.stopPortLogging();
			}
			
			observer::Registration subscribeToPortLogEvents(LogLevelWrapper const & event, std::function<void(LogLevelWrapper const & level, std::string const & domain, std::string const & str, void *data)> func)
			{
				return detail::PortLogEventHandler.registerObserver(event, std::move(func));
			}
		}
	}
}
