#include "helper_gphoto2.hpp"
#include "camera_list_wrapper.hpp"
#include "exceptions.hpp"

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
