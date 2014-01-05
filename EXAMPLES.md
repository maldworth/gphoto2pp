Examples
========

Table Of Contents
-----------------
* [Example 1](#example-1) - No device required
* [Example 2](#example-2) - Scans for connected cameras
* [Example 3](#example-3) - Connects to first camera and prints summary
* [Example 4](#example-4) - Captures some pictures
* [Example 5](#example-5) - Scans and displays settings (widgets) available
* [Example 6](#example-6) - Shows widget read and write functionality
* [Example 7](#example-7) - Preview (liveview) sample
* [Example 8](#example-8) - Preview w/manual focus sample
* [Example 9](#example-9) - Lists files/folders on camera memory card
* [Example 10](#example-10) - Tether picture capturing
* [Example 11](#example-11) - Debugging samples

You can run the examples one of two ways.
1. **gphoto2pp already installed**
   Simply compile the example with this command:
   ```sh
   g++ -std=c++11 exampleX.cpp -o exampleX -lgphoto2 -lgphoto2pp
   ```
2. **without gphoto2pp installed**
   You will need to follow the [install instructions](INSTALL.md), but most importantly when you get the the last section ([installing gphoto2pp](INSTALL.md#installing-gphoto2pp)), you don't need to run ``make install``, you can just run ``make``.
   After that completes successfully, and then you can execute any example with the ``./bin/examples/exampleX``.

The examples should have fairly detailed comments in the source code, so I will try not to repeat myself in the following sections.

Example 1
---------
Has zero device interaction, so you can actually run this just to *double check* that gphoto2 and gphoto2pp are installed and linked in your system. Each gphoto2pp member and class maps to a gphoto method, or struct with associated methods. **LibraryVersion** maps to *gp_library_version(...)*. This trend will continue, , stripping off the gp_, or gp_camera_ and then CamelCasing the proceeding gphoto2 method signature.

```cpp
#include "helper_gphoto2.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
	auto version = gphoto2pp::GetVersion();
	std::cout << "#############################" << std::endl;
	std::cout << "# Short Version - " << std::endl;
	std::cout << "#############################" << std::endl;
	std::cout << version << std::endl << std::endl;
	
	version = gphoto2pp::GetVersion(true);
	std::cout << "#############################" << std::endl;
	std::cout << "# Verbose Version - " << std::endl;
	std::cout << "#############################" << std::endl;
	std::cout << version << std::endl << std::endl;
}
```

Example 2
---------
There are a variety of gphoto2 structs (eg. Camera, CameraList, CameraFile, etc...), and so the RAII Classes I chose to append *Wrapper* to the end of the name. In this case, *struct CameraList* is contained inside our class *CameraListWrapper*, and it's created and cleaned up appropriately.

This example scans all ports for recognized cameras and returns the list of cameras found. 

You may also note we have added the 'helper_' header again. When certain actions and methods are performed that don't don't directly map to a gphoto2 method or struct, I separate them into a *helper_* header file.

Lastly we have two parent type of exceptions that can be caught.
* **gphoto2pp::exceptions::gphoto2_exception**
  Every gphoto2 method call is wrapped in a *checkResponse* method, which looks at the return value, and if it's less than 0, this means it is an error code, and so this will gather the error message and throw.
* **gphoto2pp::exceptions::GPhoto2ppExceptions**
  Everything else that might fail or cause unknown behavior that is not related to a gphoto2 call will inherit from this exception.

```cpp
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
```

Example 3
---------
This example will:
* find the first camera connected
* connect to the camera
* and lastly gets the camera summary

```cpp
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
		std::cout << "gphoto2 couldn't detect any cameras connected to the computer" << std::endl;
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
		std::cout << "gphoto2 Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
```

Example 4
---------
This examples shows a few different methods of capturing. All of them use the *helper::* which I recommend you use, and also check out [helpers documentation](http://maldworth.github.io/gphoto2pp/namespacegphoto2pp_1_1helper.html).
* Initializing the cameraWrapper will connect to the first found camera. This saves us the time of calling the autoDetect methods.
* For certain canon, setting the "capture" widget is necessary.
* Capture Method 1
  Pretty self explanitory in the code. Not much else to say.
* Capture Method 2
  Don't be intimidated by this. The whole first part just shows the name detection and manipulation helper methods. Useful but not necessary if you just want to snap a quick picture and save it. Look for the comment ``/* Simple Capture */`` and those lines are all you need to quickly detect and save the file with an appropriate name.
  Also to note the capture method is passed a boolean value of **true** which indicates that the file can be automatically removed from the camera wrapper memory. You would leave this **false** if you set your capture target to the camera SD card.
* Capture Method 3
  Shows capture into a file stream. Again, could use any type of stream, but a file suits the example the most appropriately.

```cpp
#include "helper_camera_wrapper.hpp"
#include "camera_wrapper.hpp"
#include "camera_file_wrapper.hpp"
#include "camera_capture_type_wrapper.hpp"
#include "exceptions.hpp"

#include "window_widget.hpp"
#include "toggle_widget.hpp"

#include <iostream>
#include <fstream> // only for capture method 3

// This method is only needed for certain canon cameras. I had tested this on a Canon EOS Rebel T2i (shows as Canon EOS 550D) with the boolean set to true and false, and I noticed no difference. So you might need it or you might not depending on what camera you use.
void setCanonCapture(gphoto2pp::CameraWrapper& cameraWrapper, bool capture)
{
	try
	{
		auto captureWidget = cameraWrapper.getConfig().getChildByName<gphoto2pp::ToggleWidget>("capture");
		captureWidget.setValue(capture ? 1 : 0);
		cameraWrapper.setConfig(captureWidget);
	}
	catch(const std::runtime_error& e)
	{
		// Swallow the exception
		std::cout << "Tried to set canon capture, failed. The camera is probably not a canon" << std::endl;
	}
}

int main(int argc, char* argv[]) {
	try
	{
		std::cout << "#############################" << std::endl;
		std::cout << "# connect to camera         #" << std::endl;
		std::cout << "#############################" << std::endl;
		gphoto2pp::CameraWrapper cameraWrapper; // Not passing in model and port will connect to the first available camera.
		
		setCanonCapture(cameraWrapper, true);
		
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
```

Example 5
---------
There's no point in pasting the code here, but this example basically shows off two methods:
* getAllWidgetsNames(...) -> which will return a vector<string> of all widgets in the camera
* getAllWidgetsOfType(enum type) -> which will also return a vector<string> of only widgets that are of the specified type.
Different cameras offer different abilities and configuration, which should be accessable through these widgets. Some are clearly named, some are not. You will see examples of both in example 6.

I guess this is a good time as any to briefly explain what the camera widgets are. (Full explanation of widgets can be found [here](http://maldworth.github.io/gphoto2pp/classgphoto2pp_1_1_camera_widget_wrapper.html#details).
GPhoto2 organizes the camera's settings in a n-ary tree of widgets. The widget types and their associated values are:

| Type    | Value  |
| ------- |:------:|
| Window  | N/A    |
| Section | N/A    |
| Text    | string |
| Range   | float  |
| Toggle  | int    |
| Radio   | string |
| Menu    | string |
| Button  | N/A    |
| Date    | time_t |

For example, the battery level indicator is a read only **Text** widget. The name is *batterylevel*, but the full name is */main/status/batterylevel*. This represents the tree path taken to get to the batterylevel, and *main* would be a **window** widget, and *status* is a **section** widget. Although the setValue method is accessible for all widgets, attempt to set a value to the *batterylevel* widget would result in a gphoto2_exception.

Example 6
---------
Don't be daunted by this example. Because I wanted to make them easy to interact (have a command line menu prompt), it added a bit of unnecessary complexity. I have just removed one snippit to explain below, which is all you really need to start.

In order to get widgets, you have to always first use the ``CameraWrapper::getConfig()``. This is traversing the camera settings and constructing the N-ary tree of all current settings. This is a snapshot of the camera settings. If the camera is altered externally (I press the menu button and change a setting on the camera), then I will have to call the ``getConfig`` method again, because now my current one is out of date.

Most widgets have a name, which is what we use to retrieve the widget ``getChildByName``. This *name* is the value you received from example 5. You must make sure its just the *name*. In the below example however, we use ``getChildByLabel``, but you might wonder how we knew that the label was *ISO speed*. I previously retrieved the widget, and then used ``getLabel`` to find out what the label was. Most of the time you will use ``getChildByName``, but I wanted to show you there are other ways to get the same widget.

When preparing to write to the camera, its always good to have the most recent settings. Because what you do is you are writing the ISO to the leaf node, and then writing the whole tree back to the camera. So as I mentioned above, if your tree is out of date (maybe some other leaf node has updated), then you are potentially changing another setting on the camera.

Because of this, you not only have to set the iso value on the widget, but then you have to pass the widget back into ``CameraWrapper::setConfig(...)`` in order for the settings to be saved to the camera.

```cpp
void doRadioWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Radio Widget             #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	// Now we will get the iso widget. We could use the name *iso*, but to change it up, we will use the label instead
	auto radioWidget = rootWidget.getChildByLabel<gphoto2pp::RadioWidget>("ISO Speed");
	std::cout << "\tReading iso Values" << std::endl;
	std::cout << "\t   label:\t" << radioWidget.getLabel() << std::endl;
	std::cout << "\t   info:\t " << radioWidget.getInfo() << std::endl;
	std::cout << "\t   type:\t" << static_cast<int>(radioWidget.getType()) << std::endl;
	std::cout << "\t   value:\t" << radioWidget.getValue() << std::endl;	
	std::cout << "\t   count:\t" << radioWidget.countChoices() << std::endl;	
	std::cout << "\t   choices:\t" << radioWidget.choicesToString(" ") << std::endl;
	
	if(readOnly == false)
	{
		std::cout << std::endl << "\tSetting radio..." << std::endl;
		try
		{
			radioWidget.setValue("400");
			cameraWrapper.setConfig(rootWidget);
			std::cout << "\t   done" << std::endl;
		}
		catch(gphoto2pp::exceptions::gphoto2_exception& e)
		{
			std::cout << "\tError: couldn't change the radio" << std::endl;
		}
		catch(gphoto2pp::exceptions::ValueOutOfLimits& e)
		{
			std::cout << "\tError: " << e.what() << std::endl;
		}
	}
}

```

If you also take a glance at ``void doMenuWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)`` you will notice that this widget name is *5004* which is somewhat meaningless. But if you retrieve this widget and look at the label and values, you will see that it with a Nikon D90, it represents the capture type. So if there's some setting on your camera that is more unique to the manufacturer, you might have to explore some of the oddly named widgets to find out what it does.

Example 7
---------
For cameras that support a live view mode, this will want to use [capture preview](http://maldworth.github.io/gphoto2pp/classgphoto2pp_1_1_camera_wrapper.html#ae21c7d41a07fbc4581b0b84330bc2c4d) to get a continuous stream of pictures. Please note I haven't tested this on canon cameras, it might need a little tweak to get it to work.

Example 8
---------
This will iterate twice, one slow way (because a call to getConfig each time), and then a fast way (ignoring the call to getConfig). It's best practice to perform it the slow way, as you always want to update your tree by calling ``getConfig``, but should you need a faster way, you will need to make sure each subsequent call with the same tree alters the value slightly for it to register a change.

Example 9
---------
Lists all folders and files on the camera (using the helper methods), as well as lists the first found folder not using the helper methods.

```cpp
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
```
Example 10
----------
This example shows how to make a tethered listener. GPhoto2 has an example *sample-tether.c*. This has a loop it uses for the wait_for_event, and so in C++11 we are able to easily use threads with an observer pattern. This functoinality is all baked into gphoto2pp, and all you need to do is subscribe for the event type, and start the listener.
You can use lambda functions, or if you have an instance method you want to bind the event to instead, you can use std::bind with the function and instance reference.
```cpp
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
	using namespace std::placeholders;	// for _1, _2, _3, see http://en.cppreference.com/w/cpp/utility/functional/bind for more details
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

```

Example 11
----------
This example is really meant for debugging the gphoto2pp and libgphoto2 libraries. It's useful and I recommend you read [debugging gphoto2pp](DEV-TEST.md#debugging-gphoto2pp) and [debugging libgphoto2](DEV-TEST.md#debugging-libgphoto2) sections before looking at the example11.cpp source code.
