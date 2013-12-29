#include <cxxtest/TestSuite.h>

//~ #include "GPhoto2ppTestFixture.h"

#include "helper_gphoto2.hpp"
#include "camera_list_wrapper.hpp"
#include "exceptions.hpp"
#include "log.h"

class Helpers_gphoto2_NoDevice : public CxxTest::TestSuite 
{	
public:
	void setUp()
	{
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logCRITICAL;
	}
	
	void testCheckResponseOk()
	{
		TS_ASSERT_EQUALS(gphoto2pp::checkResponse(0,"ok_method"), 0);
	}
	
	void testCheckResponseInvalid()
	{
		TS_ASSERT_THROWS(gphoto2pp::checkResponse(-1,"invalid_method"), gphoto2pp::exceptions::gphoto2_exception);
	}
	
	void testCheckResponseSilentOk()
	{
		TS_ASSERT_EQUALS(gphoto2pp::checkResponseSilent(0,"ok_method"), 0);
	}
	
	void testCheckResponseSilentInvalid()
	{
		TS_ASSERT_EQUALS(gphoto2pp::checkResponseSilent(-1,"invalid_method"), -1);
	}
	
	void testLibraryVersion()
	{
		auto version = gphoto2pp::LibraryVersion();
		
		TS_ASSERT(!version.empty());
	}
	
	void testCameraAutodetectSingle()
	{
		try
		{
			auto cameraDetails = gphoto2pp::autoDetect();
			
			// Frusterating, I must have an old version of cxxtest, as TS_SKIP is not declared
			//TS_SKIP("Camera was found, cannot complete this test");
		}
		catch(const gphoto2pp::exceptions::NoCameraFoundError& e)
		{
			TS_ASSERT(true);
		}
		catch(const std::exception& e)
		{
			TS_FAIL(e.what());
		}
	}
	
	void testCameraAutodetectMultiple()
	{
		try
		{
			auto cameraList = gphoto2pp::autoDetectAll();
			
			// Frusterating, I must have an old version of cxxtest, as TS_SKIP is not declared
			//TS_SKIP("Camera was found, cannot complete this test");
		}
		catch(const gphoto2pp::exceptions::NoCameraFoundError& e)
		{
			TS_ASSERT(true);
		}
		catch(const std::exception& e)
		{
			TS_FAIL(e.what());
		}
	}
};
