#ifndef HELPERDEBUGGING_HPP
#define HELPERDEBUGGING_HPP

#include "observer.hpp"

namespace gphoto2pp
{
	namespace helper
	{
		namespace debugging
		{
			enum class LogLevelWrapper : int
			{
				Error = 0,		// Log message is an error infomation
				Verbose = 1,	// Log message is an verbose debug infomation
				Debug = 2,		// Log message is an debug infomation
				Data = 3		// Log message is a data hex dump
			};
			
			namespace detail
			{
				struct PortLoggingEventsManager : observer::SubjectEvent<LogLevelWrapper, void(const LogLevelWrapper&, const std::string&, const std::string&, void*)>
				{
				public:
					PortLoggingEventsManager() = default;
					~PortLoggingEventsManager();
					void startPortLogging(const LogLevelWrapper& level);
					void stopPortLogging();
				private:
					int id = 0;
				};
			}
			
			void startPortLogging(const LogLevelWrapper& level);
			void stopPortLogging();
			
			observer::Registration subscribeToPortLogEvents(const LogLevelWrapper& event, std::function<void(const LogLevelWrapper& level, const std::string& domain, const std::string& str, void *data)> func);
		}
	}
}

#endif // HELPERDEBUGGING_HPP
