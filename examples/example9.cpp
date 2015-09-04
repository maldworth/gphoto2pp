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
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>

#include <gphoto2pp/exceptions.hpp>

#include <iostream>

/*
 * Example 9
 */

int main(int argc, char* argv[]) {
	
	try
	{
		std::cout << std::endl << "#############################" << std::endl;
		std::cout << "listing all files and folders may take quite some time if your memory card has a lot of pictures on it (I've had it take upwards of 30 seconds)" << std::endl;
		std::cout << "Please be patient...." << std::endl << std::endl;
		std::cout << "#############################" << std::endl;
		
		std::cout << "#############################" << std::endl;
		std::cout << "# List all folders          #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		gphoto2pp::CameraWrapper cameraWrapper;
	
		auto folders = gphoto2pp::helper::getAllFolders(cameraWrapper);
		
		for(auto i : folders)
		{
			std::cout << i << std::endl;
		}
		
		std::cout << std::endl << "#############################" << std::endl;
		std::cout << "# List all files          #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		auto files = gphoto2pp::helper::getAllFiles(cameraWrapper);
		
		for(auto i : files)
		{
			std::cout << i << std::endl;
		}
		
		std::cout << std::endl << "#############################" << std::endl;
		std::cout << "# List individual folder    #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		// The two helpers used above access this method, as well as folderListFiles
		// They are convienence methods, use this directly if you please
		auto clw = cameraWrapper.folderListFolders("/");
		
		if(clw.count() > 0)
		{
			std::cout << "'" << clw.getName(0) << "' and '" << clw.getValue(0) << "'" << std::endl;
		}
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
