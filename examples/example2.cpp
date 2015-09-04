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
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
	// runs autodetect method and returns all cameras connected to the computer
	std::cout << "#############################" << std::endl;
	std::cout << "# autoDetect - multiple     #" << std::endl;
	std::cout << "#############################" << std::endl;
	try
	{
		auto cameraList = gphoto2pp::autoDetectAll();
		
		for(int i = 0; i < cameraList.count(); ++i)
		{
			std::cout << "model: " << cameraList.getName(i) << std::endl;
		}
	}
	catch (const gphoto2pp::exceptions::NoCameraFoundError &e)
	{
		std::cout << "GPhoto couldn't detect any cameras connected to the computer" << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
