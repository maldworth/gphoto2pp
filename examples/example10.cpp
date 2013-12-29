#include "camera_wrapper.hpp"
#include "camera_file_wrapper.hpp"
#include "camera_file_type_wrapper.hpp"
#include "camera_file_path_wrapper.hpp"
#include "camera_event_type_wrapper.hpp" // For the subscribeToCameraEvent(...) method
#include "exceptions.hpp"

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
		
		// An alternate way to show handler registration
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
