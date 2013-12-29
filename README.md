gphoto2pp
=========

gphoto2pp is a C++11 wrapper for libgphoto2.

Table Of Contents
-----------------
* [Prerequisites](#prerequisites)
* [2 Minute Tutorial](#2-minute-tutorial)
* [Installation](INSTALL.md#installation)
  * [User Prerequisites](INSTALL.md#user-prerequisites)
  * [Dev/Test/Contributor Prerequisites](INSTALL.md#devtestcontributor-prerequisites)
  * [Libgphoto2 (Simple)](INSTALL.md#installing-libgphoto2-simple)
  * [Libgphoto2 (Advanced)](INSTALL.md#installing-libgphoto2-advanced)
  * [CMake](INSTALL.md#installing-cmake)
  * [Dev/Test/Contributor Only](INSTALL.md#installing-devtestcontributor-only)
  * [gphoto2pp](INSTALL.md#installing-gphoto2pp)
* [Examples](#examples)
  * [Example 1](EXAMPLES.md#example-1)
  * [Example 2](EXAMPLES.md#example-2)
  * [Example 3](EXAMPLES.md#example-3)
  * [Example 4](EXAMPLES.md#example-4)
  * [Example 5](EXAMPLES.md#example-5)
  * [Example 6](EXAMPLES.md#example-6)
  * [Example 7](EXAMPLES.md#example-7)
  * [Example 8](EXAMPLES.md#example-8)
  * [Example 9](EXAMPLES.md#example-9)
  * [Example 10](EXAMPLES.md#example-10)
  * [Example 11](EXAMPLES.md#example-11)
* [Dev/Test/Contributor](#devtest)
* [FAQ](#faq)
* [Doxygen Documentation](http://maldworth.github.io/gphoto2pp/)
* [Other Useful Links](#other-useful-links)
* [Version History](#version-history)

Prerequisites
------------
* A C++11 compiler (I've only tested g++, but others should work)
* libgphoto2 (2.4.14 or greater)
* cmake (2.8.3 or greater), *only needed to build the library once to generate libgphoto2pp.so*
  
2 Minute Tutorial
-----------------
The following snip will show you a quick demo of how easy it is to interact with your camera using gphoto2pp. Don't forget you will need to follow the [installation instructions](#installation) before you can compile this 2 minute tutorial on your computer.
Make a source file eg. ``touch testgphoto2pp.cpp``
Now put this in the source file
```cpp
#include <gphoto2pp/camera_wrapper.hpp> 		// Header for CameraWrapper
#include <gphoto2pp/camera_file_wrapper.hpp>	// Header for CameraFileWrapper
#include <gphoto2pp/helper_camera_wrapper.hpp>	// Used for helper::capture(...) method

#include <iostream>

int main()
{
	// Connects to the first camera found and initializes
	gphoto2pp::CameraWrapper camera;
	
	// Prints out the summary of your camera's abilities
	std::cout << camera.getSummary() << std::endl;
	
	// Creates empty instance of a cameraFile, which will be populated in our helper method
	gphoto2pp::CameraFileWrapper cameraFile;
	
	// Takes a picture with the camera and does all of the behind the scenes fetching
	gphoto2pp::helper::capture(camera, cameraFile);
	
	// Lastly saves the picture to your hard drive
	// Your camera might take files in different formats (bmp, raw)
	// so this extension might be wrong and you should rename your file appropriately
	cameraFile.save("my-gphoto2pp-test.jpg");
	
	return 1;
}
```

Now compile with the command:
```sh
g++ -std=c++11 testgphoto2pp.cpp -lgphoto2 -lgphoto2pp
```
and then execute it
```sh
./a.out
```

That's it! The library is certainly capable of more, which you can view at [examples](EXAMPLES.md) on our github pages.

Installation
------------
Please see the [Installation document](INSTALL.md) for details.

Examples
--------
Please see the [Examples document](EXAMPLES.md) for details about all examples provided.

Dev/Test
--------
If you would like to contribute/develop/test gphoto2pp, please see the [Dev-Test doc](DEV-TEST.md).

FAQ
---
Please see the [FAQ document](FAQ.md)

Doxygen Documentation
-----------------------
Please see this [projects github pages](http://maldworth.github.io/gphoto2pp/) for the auto generated Doxygen Page.

Other Useful Links
---------------
If you are new to gphoto2pp (and perhaps gphoto), then please view some of these resources:
* This library wraps [gphoto](http://www.gphoto.org/)

Version History
---------------
Placeholder
