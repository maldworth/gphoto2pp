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

#include <gphoto2pp/helper_widgets.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/window_widget.hpp>

#include <iostream>
#include <sstream>

// I Recommend you go read my synopsis on widgets first, found here: http://maldworth.github.io/gphoto2pp/classgphoto2pp_1_1_camera_widget_wrapper.html#details

int main(int argc, char* argv[]) {
	try
	{
		// I've made this example more complicated than it needs to be, but the purpose is to show
		// that there are many types of widgets that control the cameras, but for simplicity sake, there
		// are really just two methods being used here:
		// 1. getAllWidgetsNames(...)
		// 2. getAllWidgetsOfType(...)
		
		gphoto2pp::CameraWrapper cameraWrapper{}; // Not passing in model and port will connect to the first available camera.
		auto rootWidget = cameraWrapper.getConfig();
		bool showFullName = false;
		
		// Good ol lambda expressions
		auto getWidgetsOfType = [&](gphoto2pp::CameraWidgetTypeWrapper widgetType)
		{
			auto allWidgetsOfType = gphoto2pp::helper::getAllWidgetsNamesOfType(rootWidget, widgetType, showFullName);
			for(auto it = allWidgetsOfType.begin(); it != allWidgetsOfType.end(); ++it)
			{
				std::cout << *it << std::endl;
			}
		};
		
		std::string input = "";
		int menuNumber = 0;
		do
		{
			std::cout << std::endl << "#############################" << std::endl;
			std::cout << "# Scan Camera Widget Types  #" << std::endl;
			std::cout << "#############################" << std::endl;
			std::cout << "1. Date Widgets" << std::endl;
			std::cout << "2. Toggle Widgets" << std::endl;
			std::cout << "3. Button Widgets" << std::endl;
			std::cout << "4. Range Widgets" << std::endl;
			std::cout << "5. Menu Widgets" << std::endl;
			std::cout << "6. Radio Widgets" << std::endl;
			std::cout << "7. Text Widgets" << std::endl;
			std::cout << "8. Section Widgets" << std::endl;
			std::cout << "9. All Widgets" << std::endl;
			std::cout << "10. Toggle show full widget name (currently = " << std::boolalpha << showFullName << ")" << std::endl;
			std::cout << "11. Quit" << std::endl;
			std::cout << "Please press a number then hit <enter>: ";
			std::getline(std::cin, input);
			
			// Converts from string to number safely
			std::stringstream tempStream(input);
			if(tempStream >> menuNumber)
			{
				switch(menuNumber)
				{
					case 1: // Date Widget
					{
						std::cout << std::endl << "## Date Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Date);
						break;
					}
					case 2: // Toggle Widget
					{
						std::cout << std::endl << "## Toggle Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Toggle);
						break;
					}
					case 3: // Button Widget
					{
						std::cout << std::endl << "## Button Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Button);
						break;
					}
					case 4: // Range Widget
					{
						std::cout << std::endl << "## Range Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Range);
						break;
					}
					case 5: // Menu Widget
					{
						std::cout << std::endl << "## Menu Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Menu);
						break;
					}
					case 6: // Radio Widget
					{
						std::cout << std::endl << "## Radio Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Radio);
						break;
					}
					case 7: // Text Widget
					{
						std::cout << std::endl << "## Text Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Text);
						break;
					}
					case 8: // Section Widget
					{
						std::cout << std::endl << "## Section Widgets ##" << std::endl << std::endl;
						getWidgetsOfType(gphoto2pp::CameraWidgetTypeWrapper::Section);
						break;
					}
					case 9: // All Widgets
					{
						auto allWidgets = gphoto2pp::helper::getAllWidgetsNames(rootWidget, showFullName);
						for(auto widget : allWidgets)
						{
							std::cout << widget << std::endl;
						}
						break;
					}
					case 10: // Toggle showing full name
					{
						showFullName = !showFullName;
						std::cout << std::endl << "Full widget name " << (showFullName ? "WILL" : "will NOT") << " be visible" << std::endl << std::endl;
						break;
					}
					default:
					case 11: // Quit
					{
						std::cout << std::endl << "Exiting" << std::endl;
						break;
					}
				}
			}
			else
			{
				// They didn't input a number
				std::cout << std::endl << "Invalid entry, please input a number" << std::endl;
				menuNumber = -1;
			}
		}while(menuNumber != 11);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
