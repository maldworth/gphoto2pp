#include "helper_gphoto2.hpp"
#include "helper_debugging.hpp"
#include "exceptions.hpp"

#include "log.h"

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
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logDEBUG;
		auto pair = gphoto2pp::autoDetect();
		
		// Now we set it to error so it won't interfere with our example below
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logERROR;
		
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
			// gphoto2pp::FILE_LOG(logDEBUG) << (s.count() % 60) << "." << (us.count() % 1000000) << ": " << str;
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
