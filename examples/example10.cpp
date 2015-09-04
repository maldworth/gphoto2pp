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

#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/camera_file_type_wrapper.hpp>
#include <gphoto2pp/camera_file_path_wrapper.hpp>
#include <gphoto2pp/camera_event_type_wrapper.hpp> // For the subscribeToCameraEvent(...) method

#include <gphoto2pp/helper_camera_wrapper.hpp>

#include <gphoto2pp/exceptions.hpp>

#include <gphoto2pp/log.h>

#include <iostream>
#include <functional>

/*
 * Example 10 Tethered
 */
 
// Only used for the registration2 part, just to show how you can also bind to a class method
struct MyTestHandlerClass
{
	void mySuperSpecialHandler(const gphoto2pp::CameraFilePathWrapper& cameraFilePath, const std::string& data) const
	{
		std::cout << "My Super Special Handler was Triggered with file: " << cameraFilePath.Name << std::endl;
	}
};

int main(int argc, char* argv[]) {
	using namespace std::placeholders;	// for _1, _2, see http://en.cppreference.com/w/cpp/utility/functional/bind for more details
	try
	{
		std::cout << "#############################" << std::endl;
		std::cout << "# Tethered listener         #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		gphoto2pp::CameraWrapper cameraWrapper;
		
		// Create our event handler
		auto myhandler = [&cameraWrapper](const gphoto2pp::CameraFilePathWrapper& cameraFilePath, const std::string& data)
		{
			std::cout << "File Added: " << cameraFilePath.Name << std::endl;
			std::cout << "Downloading... " << std::endl;
			auto cameraFile = cameraWrapper.fileGet( cameraFilePath.Folder, cameraFilePath.Name, gphoto2pp::CameraFileTypeWrapper::Normal);
			cameraFile.save("example10_"+cameraFilePath.Name);
			std::cout << "Done!" << std::endl;
		};
		
		// Subscribe to the event we want with our handler
		auto registration = cameraWrapper.subscribeToCameraEvent(gphoto2pp::CameraEventTypeWrapper::FileAdded, myhandler);
		
		// An alternate way to show handler registration with an object method
		MyTestHandlerClass mthc;
		auto registration2 = cameraWrapper.subscribeToCameraEvent(gphoto2pp::CameraEventTypeWrapper::FileAdded, std::bind(&MyTestHandlerClass::mySuperSpecialHandler, std::cref(mthc), _1, _2));
		
		// Start listening for events from the camera.
		cameraWrapper.startListeningForEvents();
		
		std::string input = "";
		std::cout << "Press <enter> to quit at any time." << std::endl;
		std::cout << "Now pick up your camera and take some pictures as you normally would!" << std::endl;
		std::getline(std::cin, input);
				
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
