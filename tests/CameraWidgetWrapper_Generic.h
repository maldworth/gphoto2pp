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

#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/window_widget.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/log.h>

class CameraWidgetWrapper_Generic : public CxxTest::TestSuite 
{
	gphoto2pp::CameraWrapper _camera;

public:
	void setUp()
	{
		FILELog::ReportingLevel() = logCRITICAL;
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
