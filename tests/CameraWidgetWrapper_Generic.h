#include <cxxtest/TestSuite.h>

#include "camera_wrapper.hpp"
#include "camera_widget_type_wrapper.hpp"
#include "window_widget.hpp"
#include "exceptions.hpp"
#include "log.h"

class CameraWidgetWrapper_Generic : public CxxTest::TestSuite 
{
	gphoto2pp::CameraWrapper _camera;

public:
	void setUp()
	{
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logCRITICAL;
	}
	
	void testGetName()
	{
		auto windowWidget = _camera.getConfig();
		
		TS_ASSERT(!windowWidget.getName().empty());
	}
	
	void testMoveAssignment()
	{
		auto windowWidget = _camera.getConfig();
		auto originalName = windowWidget.getName();
		
		// Move Assignment
		auto moveassignment = std::move(windowWidget);
		
		TS_ASSERT_EQUALS(moveassignment.getName(), originalName);
		
		TS_ASSERT_THROWS(windowWidget.getName(), gphoto2pp::exceptions::gphoto2_exception);
	}
	
	void testMoveConstructor()
	{
		auto windowWidget = _camera.getConfig();
		auto originalName = windowWidget.getName();
		
		// Move Constructor
		auto moveconstructor(std::move(windowWidget));
		
		TS_ASSERT_EQUALS(moveconstructor.getName(), originalName);
		
		TS_ASSERT_THROWS(windowWidget.getName(), gphoto2pp::exceptions::gphoto2_exception); // The old value is no longer valid again
	}
	
	void testCopyAssignment()
	{
		auto windowWidget = _camera.getConfig();
		auto originalName = windowWidget.getName();
		
		// Copy Assignment
		{
			auto copyassignment = windowWidget;
			
			TS_ASSERT_EQUALS(copyassignment.getName(), originalName);
			TS_ASSERT_EQUALS(windowWidget.getName(), originalName);
		}
		
		// Now out of scope, the original file should still exist and work
		TS_ASSERT_EQUALS(windowWidget.getName(), originalName);
	}
	
	void testCopyConstructor()
	{
		auto windowWidget = _camera.getConfig();
		auto originalName = windowWidget.getName();
		
		// Copy Constructor
		{
			auto copyconstructor(windowWidget);
			
			TS_ASSERT_EQUALS(copyconstructor.getName(), originalName);
			TS_ASSERT_EQUALS(windowWidget.getName(), originalName);
		}
		
		TS_ASSERT_EQUALS(windowWidget.getName(), originalName);
	}
	
	void testGetType()
	{
		auto windowWidget = _camera.getConfig();
		
		TS_ASSERT_EQUALS(windowWidget.getType(), gphoto2pp::CameraWidgetTypeWrapper::Window);
	}
	
	
};
