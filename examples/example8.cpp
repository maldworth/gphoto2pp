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

#include <gphoto2pp/helper_camera_wrapper.hpp>
#include <gphoto2pp/camera_wrapper.hpp>

#include <gphoto2pp/window_widget.hpp>
#include <gphoto2pp/range_widget.hpp>
#include <gphoto2pp/radio_widget.hpp>
#include <gphoto2pp/toggle_widget.hpp>
#include <gphoto2pp/range_widget_range.hpp>

#include <gphoto2pp/exceptions.hpp>

#include <iostream>
#include <chrono>
#include <thread>

/*
 * This Example will take 10 preview pictures, and iterate through the manual focus range.
 */

gphoto2pp::ToggleWidget getViewfinder(gphoto2pp::WindowWidget& rootWidget)
{
	try
	{
		return rootWidget.getChildByName<gphoto2pp::ToggleWidget>("viewfinder");
	}
	catch(const gphoto2pp::exceptions::InvalidWidgetType& e)
	{
		// Swallow the exception
		std::cout << "Tried to get 'viewfinder' couldn't find, will try to get 'eosviewfinder'" << std::endl;
		
		return rootWidget.getChildByName<gphoto2pp::ToggleWidget>("eosviewfinder");
	}
}

// focusTo, -1 is Range.min, 0 is the same, and 1 is Range.max
void resetManualFocus(gphoto2pp::CameraWrapper& cameraWrapper, int focusTo)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	try
	{
		auto mfocusWidget = rootWidget.getChildByName<gphoto2pp::RangeWidget>("manualfocusdrive");
			
		if(focusTo == -1)
		{
			mfocusWidget.setValue(mfocusWidget.getRange().Min);
		}
		else if(focusTo == 1)
		{
			mfocusWidget.setValue(mfocusWidget.getRange().Max);
		}
		else
		{
			return;
		}
	}
	catch(const gphoto2pp::exceptions::InvalidWidgetType& e)
	{
		// Swallow the exception and try a RadioWidget
		auto mfocusWidget = rootWidget.getChildByName<gphoto2pp::RadioWidget>("manualfocusdrive");
		
		// TODO, need to add the focus in for canon. I don't have a canon camera to test the settings, but I will try to borrow from a friend
	}
	
	// Save the focus changes to the camera.
	cameraWrapper.setConfig(rootWidget);
	
	// Pause for half a second to let the focus motor do it's thing. This is probably way longer than necessary.
	std::chrono::milliseconds dura( 500 );
	std::this_thread::sleep_for( dura );
}

// focus valid values, -3, -2, -1, 0, 1, 2, 3
void setManualFocus(gphoto2pp::CameraWrapper& cameraWrapper, int focus)
{
	auto rootWidget = cameraWrapper.getConfig();
	
	try
	{
		auto mfocusWidget = rootWidget.getChildByName<gphoto2pp::RangeWidget>("manualfocusdrive");
		
		int focusVal = 0;
		
		switch(focus)
		{
			case -3:	focusVal = -1024;	break;
			case -2:	focusVal = -512;	break;
			case -1:	focusVal = -256;	break;
			case 0:		focusVal = 0;		break;
			case 1:		focusVal = 256;		break;
			case 2:		focusVal = 512;		break;
			case 3:		focusVal = 1024;	break;
			default:	focusVal = focus;	break;
		}
		
		mfocusWidget.setValue(focusVal);
	}
	catch(const gphoto2pp::exceptions::InvalidWidgetType& e)
	{
		// Swallow the exception and try a RadioWidget
		auto mfocusWidget = rootWidget.getChildByName<gphoto2pp::RadioWidget>("manualfocusdrive");
		
		
	}
	
	// Sends the tree changes to the camera
	cameraWrapper.setConfig(rootWidget);
	
	std::chrono::milliseconds pause_for_focus( 500 );
	std::this_thread::sleep_for( pause_for_focus );
}

int main(int argc, char* argv[]) {
	try
	{
		std::cout << "#############################" << std::endl;
		std::cout << "# Capture Preview w/m focus #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		// Setting up Gphoto
		gphoto2pp::CameraWrapper cameraWrapper;
		
		auto rootWidget = cameraWrapper.getConfig();
		
		auto viewFinder = getViewfinder(rootWidget);
		
		// First lets start with setting the range to the minimum
		// First, we have to enable the viewfinder, which will enter the camera into viewfinder mode.
		viewFinder.setValue(1);
		cameraWrapper.setConfig(viewFinder);
		
		// *NOTE* If the camera wasn't in viewfinder mode, then we would not be allowed to write the manualfocusdrive
		// 		  So it becomes apparent that some widgets depend on the camera being in certain states, and so you will have become familiar with tinkering camera settings to get things to behave as you expect
		
		// Number of iterations by default;
		int loopqty = 10;
		
		int focusStep = 2;
		
		// For My nikon D90 with the DX 35mm 1.8 lens, I've found the focus range to be about ~5000
		// So 10 iterations with the focus of 2 (translates to 512), will be about 5000, and should cover the whole lens (from close to infinity)
		// I just used the arbitrary focus values from the example in libgphoto2 (focus.c), you are free to set your own values that are smaller or larger than 256 and 1024 respectively
		
		// I set it at minimum, this should be lens independent and just setting the focus as close as possible (at least on my lens it sets it to as close as possible)
		// Then I will only need to use positive increments in the forloop to adjust the focus in the positive direction (manualfocus set value is relative)
		// You can set negative values to go backwards as well. But if you wanted to do that, I'd suggest then using the min (-1) here, and going in reverse to -5000 instead.
		resetManualFocus(cameraWrapper, -1);
		
		// The first picture on my Nikon D90 always seems to be blank, so we will take a dummy picture
		gphoto2pp::helper::capturePreview(cameraWrapper,"example8_dummy.jpg");
		
		// This loop is slower, but it shows you how the value is actually relative, and each time the getValue is zero.
		for (int i = 0; i < loopqty; i++)
		{
			// this next line is unnecessary and slows everything down, but I do it just to show that the current focus value always is '0' (At least on my Nikon D90), thus setting it is a relative increment.
			setManualFocus(cameraWrapper, focusStep);
			gphoto2pp::helper::capturePreview(cameraWrapper,"example8_preview_slower"+std::to_string(i)+"_mfval("+std::to_string(focusStep)+").jpg");
		}
		
		rootWidget = cameraWrapper.getConfig();
		
		viewFinder = getViewfinder(rootWidget);
		
		// Now we are done, we flip the mirror back down (if not already)
		viewFinder.setValue(0);
		cameraWrapper.setConfig(viewFinder);
	}
	catch (const gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
