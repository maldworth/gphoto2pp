#include "helper_debugging.hpp"

#include "helper_gphoto2.hpp"

#include "log.h"

//#include <chrono>

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
				inline std::string strformat(const char* fmt, va_list vl){
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
				
				inline std::string strformat(const char* fmt, ...){
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
				
				void PortLoggingEventsManager::startPortLogging(const LogLevelWrapper& level)
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
			
			void startPortLogging(const LogLevelWrapper& level)
			{
				detail::PortLogEventHandler.startPortLogging(level);
			}
			
			void stopPortLogging()
			{
				detail::PortLogEventHandler.stopPortLogging();
			}
			
			observer::Registration subscribeToPortLogEvents(const LogLevelWrapper& event, std::function<void(const LogLevelWrapper& level, const std::string& domain, const std::string& str, void *data)> func)
			{
				return detail::PortLogEventHandler.registerObserver(event, std::move(func));
			}
		}
	}
}
