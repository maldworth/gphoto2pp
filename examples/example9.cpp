#include "helper_camera_wrapper.hpp"
#include "camera_wrapper.hpp"
#include "camera_list_wrapper.hpp"

#include "exceptions.hpp"

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
