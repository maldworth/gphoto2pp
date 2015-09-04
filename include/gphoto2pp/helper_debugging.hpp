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

#ifndef HELPERDEBUGGING_HPP
#define HELPERDEBUGGING_HPP

#include <gphoto2pp/observer.hpp>

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
				struct PortLoggingEventsManager : observer::SubjectEvent<LogLevelWrapper, void(LogLevelWrapper const &, std::string const &, std::string const &, void*)>
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
			
			observer::Registration subscribeToPortLogEvents(const LogLevelWrapper& event, std::function<void(LogLevelWrapper const & level, std::string const & domain, std::string const & str, void *data)> func);
		}
	}
}

#endif // HELPERDEBUGGING_HPP
