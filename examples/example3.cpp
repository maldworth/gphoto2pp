#include "helper_gphoto2.hpp"
#include "camera_wrapper.hpp"
#include "exceptions.hpp"

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
