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
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
	std::string model;
	std::string port;
	
	// runs the autodetect method and returns the first camera we find (if any)
	std::cout << "#############################" << std::endl;
	std::cout << "# autoDetect - first camera #" << std::endl;
	std::cout << "#############################" << std::endl;
	try
	{
		auto cameraPair = gphoto2pp::autoDetect();
		std::cout << "model: " << cameraPair.first << " port: " << cameraPair.second << std::endl;
		
		model = cameraPair.first;
		port = cameraPair.second;
	}
	catch (gphoto2pp::exceptions::NoCameraFoundError &e)
	{
		std::cout << "GPhoto couldn't detect any cameras connected to the computer" << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
	
	try
	{
		std::cout << "connecting to the first camera (model and port above)..." << std::endl;
		auto cameraWrapper = gphoto2pp::CameraWrapper(model, port);
		
		std::cout << "#############################" << std::endl;
		std::cout << "# print camera summary      #" << std::endl;
		std::cout << "#############################" << std::endl;
		std::cout << cameraWrapper.getSummary() << std::endl;
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
