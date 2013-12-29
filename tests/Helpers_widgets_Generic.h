#include <cxxtest/TestSuite.h>

#include "helper_widgets.hpp"
#include "camera_wrapper.hpp"
#include "window_widget.hpp"
#include "exceptions.hpp"
#include "log.h"

class Helpers_gphoto2_Generic : public CxxTest::TestSuite 
{
	gphoto2pp::CameraWrapper _camera;
	
public:
	void setUp()
	{
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logCRITICAL;
	}
	
	void testGetAllWidgets()
	{
		TS_ASSERT_THROWS_NOTHING(gphoto2pp::helper::getAllWidgetsNames(_camera.getConfig()));
	}
};
