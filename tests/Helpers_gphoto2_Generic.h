#include <cxxtest/TestSuite.h>

#include "helper_gphoto2.hpp"
#include "camera_list_wrapper.hpp"
#include "exceptions.hpp"
#include "log.h"

class Helpers_gphoto2_Generic : public CxxTest::TestSuite 
{
	std::pair<std::string, std::string> _cameraDetails;
	
public:
	void setUp()
	{
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logCRITICAL;
	}
	
	void testCameraAutodetectSingle()
	{
		try
		{
			_cameraDetails = gphoto2pp::autoDetect();
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
	
	void testCameraAutodetectMultiple()
	{
		std::vector<std::string> models;
		std::vector<std::string> ports;
		try
		{
			auto cameraList = gphoto2pp::autoDetectAll();
			
			if(cameraList.count() <= 1)
			{
				// No TS_SKIP available :(
				//TS_SKIP("Only 1 Camera Found, cannot complete this test, skipping...");
			}
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
	
	
};
