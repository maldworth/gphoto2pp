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

#include <gphoto2pp/helper_camera_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/camera_wrapper.hpp>

#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {
	
	try
	{
		gphoto2pp::CameraWrapper cameraWrapper;
		
		std::cout << "#############################" << std::endl;
		std::cout << "# Capture Preview           #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		std::cout << "How Many preview pictures would you like to take [1..100]? ";
		std::string input = "";
		int pictureQty = 0;
		std::getline(std::cin, input);
		
		std::stringstream tempStream(input);
		
		if(tempStream >> pictureQty)
		{
			if(pictureQty < 0 || pictureQty > 100)
			{
				std::cout << "That number is not between 1 and 100 (inclusive)" << std::endl;
				return 0;
			}
			else
			{
				for (int i = 0; i < pictureQty; i++)
				{
					gphoto2pp::helper::capturePreview(cameraWrapper,"example7_preview"+std::to_string(i)+".jpg");
				}
				std::cout << "Done! check the directory which example7 executed in, the pictures were saved there" << std::endl;
			}
		}
		else
		{
			std::cout << "That was not a number" << std::endl;
			return 0;
		}
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
