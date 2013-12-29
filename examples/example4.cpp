#include "helper_camera_wrapper.hpp"
#include "camera_wrapper.hpp"
#include "camera_file_wrapper.hpp"
#include "camera_capture_type_wrapper.hpp"
#include "exceptions.hpp"

#include <iostream>
#include <fstream> // only for capture method 3

int main(int argc, char* argv[]) {
	try
	{
		std::cout << "#############################" << std::endl;
		std::cout << "# connect to camera         #" << std::endl;
		std::cout << "#############################" << std::endl;
		gphoto2pp::CameraWrapper cameraWrapper; // Not passing in model and port will connect to the first available camera.
		
		std::cout << "#############################" << std::endl;
		std::cout << "# Capture Method 1          #" << std::endl;
		std::cout << "#############################" << std::endl;
		std::cout << "Taking first picture..." << std::endl << std::endl;
		// Clean and quick capture and save to disk, but this assumes you are taking images, and in jpeg foramt. Adjust type and extension as appropriate.
		gphoto2pp::helper::capture(cameraWrapper, "example4_capture_method_2.jpg", true);
		
		std::cout << "#############################" << std::endl;
		std::cout << "# Capture Method 2          #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		
		std::cout << "Taking second picture..." << std::endl << std::endl;
		gphoto2pp::CameraFileWrapper cameraFileWrapper;
		gphoto2pp::helper::capture(cameraWrapper, cameraFileWrapper, true);
		
		// This is only really needed if your camera is set to take pictures in RAW.
		// Because by default the mime type for those is 'application/unknown'
		cameraFileWrapper.detectMimeType();
		
		std::cout << "FileName (empty by default): "<< cameraFileWrapper.getFileName() << std::endl;
		
		std::cout << "MimeType: "<< cameraFileWrapper.getMimeType() << std::endl;
		
		// This will only set the extension for known mime types (application/unknown is not understood, and you should perform detectMimeType before this method)
		cameraFileWrapper.adjustNameForMimeType();
		std::cout << "FileName (still empty if the mime type is still unknown): "<< cameraFileWrapper.getFileName() << std::endl;
		
		// I choose a filename
		std::string fileName("example4_capture_method_1a." + cameraFileWrapper.getFileName());
		
		// Just for the purpose of demonstration, we will set the extension to something random
		// Again, the release notes say that the setFileName and getFileName will be going away, which I understand,
		// because it's not until we call the .save(fileName) method, that it's saved to the place with the directory.
		cameraFileWrapper.setFileName("TestRandom0293.odf");
		
		std::cout << "Delibrately incorrect filename and extension: "<< cameraFileWrapper.getFileName() << std::endl;
		
		// Should Fix the extension
		cameraFileWrapper.adjustNameForMimeType();
		
		std::cout << "FileName (proper extension): "<< cameraFileWrapper.getFileName() << std::endl;
		
		// Now after all that fiddling with filenames, we save the file and use our own filename (haha)
		std::cout << "FileName we will save with: "<< fileName << std::endl;
		cameraFileWrapper.save(fileName);
		
		// Now lets take another picture but ignore all of that messing around we did above and just use 4 lines of code instead :)
		std::cout << std::endl << "Taking third picture..." << std::endl << std::endl;
		//cameraFileWrapper = cameraWrapper.capture();
		gphoto2pp::helper::capture(cameraWrapper, cameraFileWrapper, true);
		cameraFileWrapper.detectMimeType();
		cameraFileWrapper.adjustNameForMimeType();
		cameraFileWrapper.save("example4_capture_method_1b."+cameraFileWrapper.getFileName());
		
		std::cout << "#############################" << std::endl;
		std::cout << "# Capture Method 3          #" << std::endl;
		std::cout << "#############################" << std::endl;
		std::cout << "Taking fourth picture..." << std::endl << std::endl;
		std::fstream f1("example4_capture_method_3.jpg",std::fstream::out|std::fstream::binary|std::fstream::trunc);
		gphoto2pp::helper::capture(cameraWrapper,f1, true);
		f1.close();
		
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
