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

#include <cxxtest/TestSuite.h>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/helper_widgets.hpp> // for the getAllWidgetsOfType
#include <gphoto2pp/camera_wrapper.hpp>

#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/window_widget.hpp>
#include <gphoto2pp/radio_widget.hpp>
#include <gphoto2pp/toggle_widget.hpp>
#include <gphoto2pp/range_widget.hpp>
#include <gphoto2pp/text_widget.hpp>
#include <gphoto2pp/menu_widget.hpp>

#include <gphoto2pp/camera_capture_type_wrapper.hpp>
#include <gphoto2pp/camera_file_path_wrapper.hpp>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/log.h>

class NikonD90 : public CxxTest::TestSuite 
{
	gphoto2pp::CameraWrapper _camera;
public:

	void setUp()
	{
		FILELog::ReportingLevel() = logCRITICAL;
	}

	void testNikonD90()
	{
		try
		{
			auto cameraDetails = gphoto2pp::autoDetect();
			
			TS_ASSERT_EQUALS( cameraDetails.first, "Nikon DSC D90 (PTP mode)" );
		}
		catch(const gphoto2pp::exceptions::NoCameraFoundError& e)
		{
			TS_FAIL("No Camera detected, please make sure your camera is on and plugged in");
		}
		catch(const std::exception& e)
		{
			TS_FAIL(e.what());
		}
	}
	
	void testRetrievalWrongWidgetType()
	{
		auto rootWidget = _camera.getConfig();
		
		// This is a delibrately incorrect call and should throw
		TS_ASSERT_THROWS(rootWidget.getChildByName<gphoto2pp::ToggleWidget>("iso"), gphoto2pp::exceptions::InvalidWidgetType);
	}
	
	void testSetWrongChoiceIndex()
	{
		auto isoWidget = _camera.getConfig().getChildByName<gphoto2pp::RadioWidget>("iso");
		
		TS_ASSERT_THROWS(isoWidget.setChoice(99), gphoto2pp::exceptions::IndexOutOfRange);
	}
	
	void testISO_RadioWidget()
	{
		// First lets get the current iso, and save the current setting, so we can restore the camera to the settings we found it at.
		auto isoWidget = _camera.getConfig().getChildByName<gphoto2pp::RadioWidget>("iso");
		auto oldIso = isoWidget.getChoice();
		
		// Now lets determine the ISO we will be setting it to and change it
		// 0 should be ISO=100, and 3 should be ISO=200
		auto setToISO = oldIso == 0 ? 3 : 0;
		
		TS_ASSERT_THROWS_NOTHING(isoWidget.setChoice(setToISO));
		
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(isoWidget));
		
		// Now lets get the new config to make sure it was written
		isoWidget = _camera.getConfig().getChildByName<gphoto2pp::RadioWidget>("iso");
		
		// Confirm that it was set properly
		TS_ASSERT_DIFFERS(oldIso, isoWidget.getChoice());
		
		// Now lets change it back to the original value
		TS_ASSERT_THROWS_NOTHING(isoWidget.setChoice(oldIso));
		
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(isoWidget));
		
		// Confirm it was changed back
		TS_ASSERT_EQUALS(oldIso, isoWidget.getChoice());
	}
	
	void testAutoFocus_ToggleWidget()
	{
		// Before we trigger autofocus, we make sure this lens supports AF-S
		auto focusMode2 = _camera.getConfig().getChildByName<gphoto2pp::RadioWidget>("focusmode2");
		
		if(focusMode2.getValue() == "MF (fixed)" || focusMode2.getValue() == "MF (selection)")
		{
			TS_WARN("Lens either doesn't support AF, or switch is set to Manual");
			TS_ASSERT(1)
		}
		else
		{
			TS_ASSERT_DIFFERS(focusMode2.getValue(),"MF (fixed)")
			TS_ASSERT_DIFFERS(focusMode2.getValue(),"MF (selection)")
			
			// Great, now we know the lens supports auto focus and it's on either AF-S, AF-C or AF-A
			auto afsWidget = _camera.getConfig().getChildByName<gphoto2pp::ToggleWidget>("autofocusdrive");
			
			// Toggling 1 makes an AF lens auto focus. Not sure what happens if it's a manual focus lens, or the m/f switch is triggered.
			// Could make test more robust by putting try{} catch{} around it, and if it fails, we read some value to determine if the lens is set into M/F mode
			TS_ASSERT_THROWS_NOTHING(afsWidget.setValue(true));
			TS_ASSERT_THROWS_NOTHING(_camera.setConfig(afsWidget));
		}
	}
	
	void testFlashExposure_RangeWidget()
	{
		// First lets get the current flashexposurecompensation, and save the current setting, so we can restore the camera to the settings we found it at.
		auto exposureWidget = _camera.getConfig().getChildByName<gphoto2pp::RangeWidget>("flashexposurecompensation");
		auto oldExposure = exposureWidget.getValue();
		
		// Range for exposure is -3 to 1, steps of 0.333 (or 1/3)
		auto setToExposure = oldExposure == 0 ? -3 : 0;
		
		TS_ASSERT_THROWS_NOTHING(exposureWidget.setValue(setToExposure));
		
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(exposureWidget));
		
		// Now lets get the new config to make sure it was written
		exposureWidget = _camera.getConfig().getChildByName<gphoto2pp::RangeWidget>("flashexposurecompensation");
		
		// Confirm that it was set properly
		TS_ASSERT_DIFFERS(oldExposure, exposureWidget.getValue());
		
		// Now lets change it back to the original value
		TS_ASSERT_THROWS_NOTHING(exposureWidget.setValue(oldExposure));
		
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(exposureWidget));
		
		// Confirm it was changed back
		TS_ASSERT_EQUALS(oldExposure, exposureWidget.getValue());
	}
	
	void testImageQuality_MenuWidget()
	{
		// 5004 Corresponds to Nikon D90 Image Type menu, 0=JPEG basic, 1=JPEG normal, 2=JPEG fine, 4=RAW, 5=RAW+basic, 6=RAW+normal, 7=RAW+fine
		// 5003 Corresponds to Nikon D90 Image Size menu (only relevant for 5004 options with Jpeg in it)
		
		// First lets get the current image quality and image size values and store them,so we can restore the camera after testing is complete.
		
		// Because we are making two calls, we will store the rootWidget in a pointer to avoid gphoto from having to construct the tree twice.
		auto rootWidget = _camera.getConfig();
		auto imageTypes = rootWidget.getChildByName<gphoto2pp::MenuWidget>("5004");
		
		auto oldImageType = imageTypes.getValue();
		
		// Value to set our camera to
		auto setToType = oldImageType == "0" ? "5" : "0";
		
		// If it's raw, we need to set the camera's type to Jpeg Basic first before we can set the size.
		TS_ASSERT_THROWS_NOTHING(imageTypes.setValue(setToType));
		
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(imageTypes));
		
		// Now we update our config tree, so we have the latest from the camera
		rootWidget = _camera.getConfig();
		
		
		// Next we get our size value
		auto imageSizes = rootWidget.getChildByName<gphoto2pp::MenuWidget>("5003");
		
		// Save the old image size, so we can restore it
		auto oldImageSize = imageSizes.getValue();
		
		// Range for exposure is -3 to 1, steps of 0.333 (or 1/3)
		
		auto setToSize = oldImageSize == "2144x1424" ? "3216x2136" : "2144x1424";
		
		// Setting Size
		TS_ASSERT_THROWS_NOTHING(imageSizes.setValue(setToSize));
		
		// Saving the settings to the camera, we can use either widget
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(imageSizes));
		
		// Now lets get the new config to make sure it was written
		rootWidget = _camera.getConfig();
		imageTypes = rootWidget.getChildByName<gphoto2pp::MenuWidget>("5004");
		imageSizes = rootWidget.getChildByName<gphoto2pp::MenuWidget>("5003");
		
		// Confirm that it was set properly
		TS_ASSERT_DIFFERS(oldImageType, imageTypes.getValue());
		TS_ASSERT_DIFFERS(oldImageSize, imageSizes.getValue());
		
		// Now lets change it back to the original value
		TS_ASSERT_THROWS_NOTHING(imageTypes.setValue(oldImageType));
		TS_ASSERT_THROWS_NOTHING(imageSizes.setValue(oldImageSize));
		
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(imageTypes));
		
		// Confirm it was changed back
		TS_ASSERT_EQUALS(oldImageType, imageTypes.getValue());
		TS_ASSERT_EQUALS(oldImageSize, imageSizes.getValue());
	}
	
	void testBatteryLevel_TextWidget()
	{
		auto batteryLevelWidget = _camera.getConfig().getChildByName<gphoto2pp::TextWidget>("batterylevel");
		
		TS_ASSERT(!batteryLevelWidget.getValue().empty());
	}
	
	void testActions_SectionWidget()
	{
		auto actionWidget = _camera.getConfig().getChildByName<gphoto2pp::NonValueWidget>("actions");
		
		TS_ASSERT_LESS_THAN_EQUALS(3, actionWidget.countChildren()); // (because 2.4.14 has 3 children, and 2.5 or greater has 4 children, actually 2.5.3 has 5 children)
	}
	
	// This is the test that sets the capturetarget widget to SD instead of internal memory
	void testCaptureToSD()
	{
		auto radioWidget = _camera.getConfig().getChildByName<gphoto2pp::RadioWidget>("capturetarget");
		
		// Sets picture capture to memory card
		TS_ASSERT_THROWS_NOTHING(radioWidget.setValue("Memory card"));
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(radioWidget));
		
		auto filePath = _camera.capture(gphoto2pp::CameraCaptureTypeWrapper::Image);
		
		auto defaultCaptureName = "_DSC";
		
		TS_ASSERT(filePath.Name.find(defaultCaptureName) != std::string::npos);
		
		// Sets picture capture back to internal RAM
		TS_ASSERT_THROWS_NOTHING(radioWidget.setValue("Internal RAM"));
		TS_ASSERT_THROWS_NOTHING(_camera.setConfig(radioWidget));
	}
	
	// TODO: Test for movie record
	
	// Some additional tests for helpers
	void testgetAllByWidgetType()
	{
		auto widgets = gphoto2pp::helper::getAllWidgetsNamesOfType(_camera.getConfig(), gphoto2pp::CameraWidgetTypeWrapper::Section);
		
		TS_ASSERT_EQUALS(widgets.size(), 6);
		
		TS_ASSERT_EQUALS(widgets.at(0),"actions");
		TS_ASSERT_EQUALS(widgets.at(5),"other");
	}
	
	void testgetAllByWidgetTypeFullname()
	{
		auto widgets = gphoto2pp::helper::getAllWidgetsNamesOfType(_camera.getConfig(), gphoto2pp::CameraWidgetTypeWrapper::Section, true);
		
		TS_ASSERT_EQUALS(widgets.size(), 6);
		
		TS_ASSERT_EQUALS(widgets.at(0),"/main/actions");
		TS_ASSERT_EQUALS(widgets.at(5),"/main/other");
	}
};
