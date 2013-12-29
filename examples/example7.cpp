#include "helper_camera_wrapper.hpp"
#include "exceptions.hpp"
#include "camera_wrapper.hpp"

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
