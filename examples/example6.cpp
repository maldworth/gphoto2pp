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

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/camera_wrapper.hpp>

//Widget types
#include <gphoto2pp/date_widget.hpp>
#include <gphoto2pp/toggle_widget.hpp>
#include <gphoto2pp/range_widget.hpp>
#include <gphoto2pp/menu_widget.hpp>
#include <gphoto2pp/radio_widget.hpp>
#include <gphoto2pp/text_widget.hpp>
#include <gphoto2pp/window_widget.hpp>

#include <iostream>
#include <sstream>

void doDateWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Date Widget               #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	auto dateWidget = rootWidget.getChildByName<gphoto2pp::DateWidget>("datetime"); // we know the widget name is 'datetime' by executing example5 and finding out the camera's available date widgets
	std::cout << "\tReading datetime Values" << std::endl;
	std::cout << "\t   label:\t" << dateWidget.getLabel() << std::endl;
	std::cout << "\t   info:\t " << dateWidget.getInfo() << std::endl; // Seems to always be blank
	std::cout << "\t   type:\t" << static_cast<int>(dateWidget.getType()) << std::endl; // c++11 strongly typed enums used
	
	auto cameraTime = dateWidget.getValue();
	std::cout << "\t   value:\t" << cameraTime << std::endl;
	std::cout << "\t   nice format:\t" << std::ctime(&cameraTime) << std::endl;
	
	if(readOnly == false)
	{
		std::cout << std::endl << "\tSetting date to Mar 14 2010" << std::endl;
		dateWidget.setValue(1268579366);
		std::cout << "\t   date set, now reading it back..." << std::endl;
		cameraTime = dateWidget.getValue();
		std::cout << "\t   nice format:\t" << std::ctime(&cameraTime) << std::endl;
		std::cout << "\t   now writing the date to camera..." << std::endl;
		cameraWrapper.setConfig(rootWidget);
		std::cout << "\t   done" << std::endl;
	}
}

void doToggleWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Toggle Widget             #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	auto toggleWidget = rootWidget.getChildByName<gphoto2pp::ToggleWidget>("autofocusdrive");
	std::cout << "\tReading autofocusdrive Values" << std::endl;
	std::cout << "\t   label:\t" << toggleWidget.getLabel() << std::endl;
	std::cout << "\t   info:\t " << toggleWidget.getInfo() << std::endl;
	std::cout << "\t   type:\t" << static_cast<int>(toggleWidget.getType()) << std::endl;
	std::cout << "\t   value:\t" << toggleWidget.getValue() << std::endl;	
	
	if(readOnly == false)
	{
		// Toggling this to 1 makes the lens perform the autofocus. Not sure what happens for mf lens. probably nothing...
		std::cout << std::endl << "\tToggling auto focus" << std::endl;
		toggleWidget.setValue(true);
		try
		{
			cameraWrapper.setConfig(rootWidget);
			std::cout << "\t   done" << std::endl;
		}
		catch(gphoto2pp::exceptions::gphoto2_exception& e)
		{
			std::cout << "\tError: couldn't auto focus on an object" << std::endl;
		}
	}
}

void doButtonWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	std::cout << std::endl << "Nikon D90 unfortunately does not have any button widgets, but you are free to put your own test implementation here!" << std::endl;
}

void doRangeWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Range Widget             #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	//Other range ones: autowhitebias, manualfocusdrive, burstnumber, flashexposurecompensation, lightmeter, cloudywhitebias, flashwhitebias, lowlight, shadewhitebias, focallength, flexibleprogram
	auto rangeWidget = rootWidget.getChildByName<gphoto2pp::RangeWidget>("flashexposurecompensation");
	std::cout << "\tReading flashexposurecompensation Values" << std::endl;
	std::cout << "\t   label:\t" << rangeWidget.getLabel() << std::endl;
	std::cout << "\t   info:\t " << rangeWidget.getInfo() << std::endl;
	std::cout << "\t   type:\t" << static_cast<int>(rangeWidget.getType()) << std::endl;
	std::cout << "\t   value:\t" << rangeWidget.getValue() << std::endl;	
	std::cout << "\t   range:\t" << rangeWidget.ToString() << std::endl;
	
	if(readOnly == false)
	{
		std::cout << std::endl << "\tSetting range..." << std::endl;
		try
		{
			rangeWidget.setValue(3);
			cameraWrapper.setConfig(rootWidget);
			std::cout << "\t   done" << std::endl;
		}
		catch(gphoto2pp::exceptions::gphoto2_exception& e)
		{
			std::cout << "\tError: couldn't change the range" << std::endl;
		}
		catch(gphoto2pp::exceptions::ValueOutOfLimits& e)
		{
			std::cout << "\tError: " << e.what() << std::endl;
		}
	}
}

void doMenuWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Menu Widget             #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	// 5004 Corresponds to Nikon D90 Image type menu, 0=JPEG basic, 1=JPEG normal, 2=JPEG fine, 4=RAW, 5=RAW+basic, 6=RAW+normal, 7=RAW+fine
	// 5003 Corresponds to Nikon D90 Image size menu (only relevant for 5004 options with Jpeg in it)
	
	auto menuWidget = rootWidget.getChildByName<gphoto2pp::MenuWidget>("5004");
	std::cout << "\tReading 5004 Values" << std::endl;
	std::cout << "\t   label:\t" << menuWidget.getLabel() << std::endl;
	std::cout << "\t   info:\t " << menuWidget.getInfo() << std::endl;
	std::cout << "\t   type:\t" << static_cast<int>(menuWidget.getType()) << std::endl;
	std::cout << "\t   value:\t" << menuWidget.getValue() << std::endl;	
	std::cout << "\t   choices:\t" << menuWidget.choicesToString(" ") << std::endl;
	
	if(readOnly == false)
	{
		std::cout << std::endl << "\tSetting it to Jpeg Basic..." << std::endl;
		
		try
		{
			menuWidget.setValue("0"); // Setting it to RAW only
			cameraWrapper.setConfig(rootWidget);
			std::cout << "\t   done, now setting 5003 (the image size) to small" << std::endl;
			auto menuWidget5003 = rootWidget.getChildByName<gphoto2pp::MenuWidget>("5003");
			menuWidget5003.setValue("2144x1424");
			cameraWrapper.setConfig(rootWidget);
			std::cout << "\t   done" << std::endl;
		}
		catch(gphoto2pp::exceptions::gphoto2_exception& e)
		{
			std::cout << "\tError: couldn't change the menu" << std::endl;
		}
		catch(gphoto2pp::exceptions::ValueOutOfLimits& e)
		{
			std::cout << "\tError: " << e.what() << std::endl;
		}
	}
}

void doRadioWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Radio Widget             #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	// Now we will get the iso widget. We could use the name 'iso', but to change it up, we will use the label instead
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

void doTextWidget(gphoto2pp::CameraWrapper& cameraWrapper, bool readOnly)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Text Widget             #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	auto textWidget = rootWidget.getChildByName<gphoto2pp::TextWidget>("batterylevel");
	std::cout << "\tReading battery level Values" << std::endl;
	std::cout << "\t   label:\t" << textWidget.getLabel() << std::endl;
	std::cout << "\t   info:\t " << textWidget.getInfo() << std::endl;
	std::cout << "\t   type:\t" << static_cast<int>(textWidget.getType()) << std::endl;
	std::cout << "\t   value:\t" << textWidget.getValue() << std::endl;	
	
	if(readOnly == false)
	{
		std::cout << std::endl << "\tSetting text..." << std::endl;
		try
		{
			// This should fail, we aren't allowed to change this text
			textWidget.setValue("80%");
			cameraWrapper.setConfig(rootWidget);
			std::cout << "\t   done" << std::endl;
		}
		catch(gphoto2pp::exceptions::gphoto2_exception& e)
		{
			std::cout << "\tError: couldn't change the text" << std::endl;
		}
	}
}

void doSectionWidget(gphoto2pp::CameraWrapper& cameraWrapper)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	std::cout << std::endl << "#############################" << std::endl;
	std::cout << "# Section Widget            #" << std::endl;
	std::cout << "#############################" << std::endl;
	
	auto sectionWidget = rootWidget.getChildByName<gphoto2pp::NonValueWidget>("actions");
	std::cout << "\tReading actions widget" << std::endl;
	std::cout << "\t   count:\t" << sectionWidget.countChildren() << std::endl;
	
	// That's it, nothing else really.
	// We could loop through all the children, from 0 to countChildren(), and then determine each child type and create the widget, but that's an exercise you can complete yourself if you so wish.
}

int main(int argc, char* argv[]) {
	//~ FILELog::ReportingLevel() = logWARN;
	
	try
	{
		// Now if you run example5 on your camera, you can see all of the widgets that are available
		// for you to congifure and set. This is important for the purpose of this example.
		// For my Nikon D90, I will be getting and setting one of each type of widget (where possible)
		// 1. Date Widget -		datetime
		// 2. Toggle Widget -	autofocusdrive
		// 3. Button Widget -	(Nikon D90 has none, so I cannot do this one)
		// 4. Range Widget -	flashexposurecompensation
		// 5. Menu Widget -		5004 and 5003 (I will be using and setting both of these)
		// 6. Radio Widget -	iso
		// 7. Text Widget -		batterylevel
		// 8. Section Widget -	actions
		
		
		//I'm using a nikon D90, if your camera is of another brand, some of the commands below might behave differently, and change different settings on your camera.
		
		gphoto2pp::CameraWrapper cameraWrapper{};
		
		bool readOnly = true;
		
		std::string input = "";
		int menuNumber = 0;
		do
		{
			std::cout << std::endl << "#############################" << std::endl;
			std::cout << "# R/W with Camera Widgets   #" << std::endl;
			std::cout << "#############################" << std::endl;
			std::cout << "Please note that some of these tests might change settings on your camera (iso, date, etc...). Please read through the example6.cpp for details" << std::endl;
			std::cout << "1. Date Widget Test" << std::endl;
			std::cout << "2. Toggle Widget Test" << std::endl;
			std::cout << "3. Button Widget Test" << std::endl;
			std::cout << "4. Range Widget Test" << std::endl;
			std::cout << "5. Menu Widget Test" << std::endl;
			std::cout << "6. Radio Widget Test" << std::endl;
			std::cout << "7. Text Widget Test" << std::endl;
			std::cout << "8. Section Widget Test" << std::endl;
			std::cout << "9. Toggle Read vs ReadWrite (currently = " << (readOnly ? "read only" : "read write") << ")" << std::endl;
			std::cout << "10. Quit" << std::endl;
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
						doDateWidget(cameraWrapper, readOnly);
						break;
					}
					case 2: // Toggle Widget
					{
						doToggleWidget(cameraWrapper, readOnly);
						break;
					}
					case 3: // Button Widget
					{
						doButtonWidget(cameraWrapper, readOnly);
						break;
					}
					case 4: // Range Widget
					{
						doRangeWidget(cameraWrapper, readOnly);
						break;
					}
					case 5: // Menu Widget
					{
						doMenuWidget(cameraWrapper, readOnly);
						break;
					}
					case 6: // Radio Widget
					{
						doRadioWidget(cameraWrapper, readOnly);
						break;
					}
					case 7: // Text Widget
					{
						doTextWidget(cameraWrapper, readOnly);
						break;
					}
					case 8: // Section Widget
					{
						doSectionWidget(cameraWrapper);
						break;
					}
					case 9: // Toggle showing full name
					{
						readOnly = !readOnly;
						std::cout << std::endl << "Widget tests will now " << (readOnly ? "read only" : "read write") << std::endl << std::endl;
						break;
					}
					default:
					case 10: // Quit
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
		}while(menuNumber != 10);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
