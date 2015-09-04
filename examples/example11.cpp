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

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/helper_debugging.hpp>
#include <gphoto2pp/exceptions.hpp>

#include <gphoto2pp/log.h>

#include <iostream>
#include <chrono>

/*
 * Example 11 - Getting debugging logs from gphoto2
 */

int main(int argc, char* argv[]) {
	try
	{
		std::cout << "#############################" << std::endl;
		std::cout << "# Debugger Usage            #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		// Enable the GPhoto2pp logging, which by default goes to the stdout
		FILELog::ReportingLevel() = logDEBUG;
		auto pair = gphoto2pp::autoDetect();
		
		// Now we set it to error so it won't interfere with our example below
		FILELog::ReportingLevel() = logERROR;
		
		// Hook the debugger up at any time, first we need a functor to send the logger for events
		auto mylogger = [](gphoto2pp::helper::debugging::LogLevelWrapper level, const std::string& domain, const std::string& str, void* data)
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
			auto s = std::chrono::duration_cast<std::chrono::seconds>(us);
			std::cout << (s.count() % 60) << "." << (us.count() % 1000000) << ": ";
			
			// This line is really the only important one, the chrono stuff is so it mimics the gphoto2 debugger output
			std::cout << str << std::endl;
			
			// If you want to log it using the same mechanism as gphoto2pp, use this line instead of cout
			// FILE_LOG(logDEBUG) << (s.count() % 60) << "." << (us.count() % 1000000) << ": " << str;
		};
		
		// This registration variable should be kept in scope for as long as you want it to log.
		// Once the registration variable falls out of scope (and is cleaned up), the handler loses registration.
		auto registration = gphoto2pp::helper::debugging::subscribeToPortLogEvents(gphoto2pp::helper::debugging::LogLevelWrapper::Debug, mylogger);
		
		// Don't start actual listening until this is triggered
		gphoto2pp::helper::debugging::startPortLogging(gphoto2pp::helper::debugging::LogLevelWrapper::Debug);
		
		// Calling any method should generate log messages, but I just happened to choose autoDetect :)
		pair = gphoto2pp::autoDetect();
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
