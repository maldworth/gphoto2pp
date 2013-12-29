#include "helper_camera_wrapper.hpp"
#include "camera_wrapper.hpp"

#include "window_widget.hpp"
#include "range_widget.hpp"
#include "toggle_widget.hpp"
#include "range_widget_range.hpp"

#include "exceptions.hpp"

#include <iostream>
#include <chrono>
#include <thread>

/*
 * This Example will take 20 preview pictures, and iterate through the manual focus range.
 */

int main(int argc, char* argv[]) {
	
	try
	{
		std::cout << "#############################" << std::endl;
		std::cout << "# Capture Preview w/m focus #" << std::endl;
		std::cout << "#############################" << std::endl;
		
		// Number of iterations by default;
		int loopqty = 10;
		
		// Nikon D90 top end range on my testing lens Nikon DX 35mm F1.8. It may vary for different lenses and cameras.
		int lensRange = 5000;
		
		// Setting up Gphoto
		gphoto2pp::CameraWrapper cameraWrapper;
		auto rootWidget = cameraWrapper.getConfig();
		auto mfocusWidget = rootWidget.getChildByName<gphoto2pp::RangeWidget>("manualfocusdrive");
		auto viewFinder = rootWidget.getChildByName<gphoto2pp::ToggleWidget>("viewfinder"); // Canon users might need to use "eosviewfinder"
		
		// Nikon D90 is [min: -32767 max: 32767 step: 1], but this below code should work for any camera that has a manual focus drive
		auto range = mfocusWidget.getRange();
		
		// My manually calculated steps which will get from the focus at 0 range to the focus at infinity range on my Lens.
		int mfstep = lensRange / loopqty;
		
		// First lets start with setting the range to the minimum
		// First, we have to enable the viewfinder, which will enter the camera into viewfinder mode.
		viewFinder.setValue(1);
		cameraWrapper.setConfig(viewFinder);
		
		// *NOTE* If the camera wasn't in viewfinder mode, then we would not be allowed to write the manualfocusdrive
		// 		  So it becomes apparent that some widgets depend on the camera being in certain states, and so you will have become familiar with tinkering camera settings to get things to behave as you expect
		
		// I set it at minimum, this should be lens independent and just setting the focus as close as possible (at least on my lens it sets it to as close as possible)
		// Then I will only need to use positive increments in the forloop to adjust the focus in the positive direction (manualfocus set value is relative)
		// You can set negative values to go backwards as well. But if you wanted to do that, I'd suggest then using range.Max here, and going in reverse to -5000 instead.
		mfocusWidget.setValue(range.Min);
		
		// And just for the purpose of example, notice I'm doing setConfig with the viewFinder widget. They are interchangable when calling setConfig.
		// Remember, that when we did getConfig() at the top, we got an N-ary tree.
		// Then when we got the mfocus and viewfinder widgets, they are both leaf nodes in the SAME tree.
		// So when we run setConfig, this method will actually get the parent most widget (calls the method CameraWidgetWrapper::getRoot()), and then writes it to the camera.
		cameraWrapper.setConfig(viewFinder);
		
		std::chrono::milliseconds dura( 500 );
		std::chrono::milliseconds pause_for_focus( 50 ); // To give the camera motor time to adjust, otherwise we might capture the preview too quickly afterwards.
		std::this_thread::sleep_for( dura );
		
		// The first picture on my Nikon D90 always seems to be blank, so we will take a dummy picture
		gphoto2pp::helper::capturePreview(cameraWrapper,"example8_dummy.jpg");
		
		// This loop is slower, but it shows you how the value is actually relative, and each time the getValue is zero.
		for (int i = 0; i < loopqty; i++)
		{
			// this next line is unnecessary and slows everything down, but I do it just to show that the current focus value always is '0' (At least on my Nikon D90), thus setting it is a relative increment.
			mfocusWidget = cameraWrapper.getConfig().getChildByName<gphoto2pp::RangeWidget>("manualfocusdrive");
			std::cout << "Current Focus Value["<< mfocusWidget.getValue() <<"] -> New Focus Value[" << mfstep <<"]" << std::endl;
			mfocusWidget.setValue(mfstep);
			cameraWrapper.setConfig(mfocusWidget);
			std::this_thread::sleep_for( pause_for_focus );
			gphoto2pp::helper::capturePreview(cameraWrapper,"example8_preview_slower"+std::to_string(i)+"_mfval("+std::to_string(mfstep)+").jpg");
		}
		
		
		std::cout << "# Now the faster loop #" << std::endl;
		
		// ### Now this loop is a faster example of what you could do if you wanted to loop quickly ###
		
		// Reset the range back to the minimum
		mfocusWidget.setValue(range.Min);
		cameraWrapper.setConfig(mfocusWidget);
		
		// Sleep for half a second to give time for the focus to go all the way back (a larger jump)
		std::this_thread::sleep_for( dura );
		
		for (int i = 0; i < loopqty; i++)
		{
			// We have to alternate between 500 and 501 because we are not doing a getConfig each time
			// and so if we just write 500 every time, the widget wouldn't know the value has changed and then it wouldn't change the focus
			// Actually now that I think about it, I need to ask Marcus if gphoto2's widget tree internal structure has some representation of a dirty flag/bit.
			mfocusWidget.setValue(mfstep + (i%2));
			cameraWrapper.setConfig(mfocusWidget);
			std::this_thread::sleep_for( pause_for_focus );
			gphoto2pp::helper::capturePreview(cameraWrapper,"example8_preview_faster"+std::to_string(i)+"_mfval("+std::to_string(mfocusWidget.getValue())+").jpg");
		}
		
		
		// Now we are done, we flip the mirror back down (if not already)
		viewFinder.setValue(0);
		cameraWrapper.setConfig(viewFinder);
	}
	catch (gphoto2pp::exceptions::gphoto2_exception& e)
	{
		std::cout << "GPhoto Exception Code: " << e.getResultCode() << std::endl;
		std::cout << "Exception Message: " << e.what() << std::endl;
	}
}
