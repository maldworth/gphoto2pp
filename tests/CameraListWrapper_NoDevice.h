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
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/log.h>

class CameraListWrapper_NoDevice : public CxxTest::TestSuite 
{
public:
	void setUp()
	{
		FILELog::ReportingLevel() = logCRITICAL;
	}
	
	void testConstructionAndAssignmentWithReset()
	{
		gphoto2pp::CameraListWrapper cameraList;
		
		cameraList.append("First Model","Some Port1");
		cameraList.append("Second Model","Some Port2");
		
		TS_ASSERT_EQUALS(cameraList.getName(0),"First Model");
		TS_ASSERT_EQUALS(cameraList.getName(1),"Second Model");
		TS_ASSERT_EQUALS(cameraList.getValue(0),"Some Port1");
		TS_ASSERT_EQUALS(cameraList.getValue(1),"Some Port2");
		
		cameraList.reset();
		
		TS_ASSERT_EQUALS(cameraList.count(),0);
	}
	
	void testConstructionAndReAssignmentWithSort()
	{
		gphoto2pp::CameraListWrapper cameraList;
		
		cameraList.append("First Model","Some Port1");
		cameraList.append("Second Model","Some Port2");
		cameraList.setName(0,"z First Model Modified");
		cameraList.setValue(1,"Some Port2 Modified");
		
		cameraList.sort();
		
		TS_ASSERT_EQUALS(cameraList.getName(0),"Second Model");
		TS_ASSERT_EQUALS(cameraList.getValue(0),"Some Port2 Modified");
		
		TS_ASSERT_EQUALS(cameraList.getName(1),"z First Model Modified");
		TS_ASSERT_EQUALS(cameraList.getValue(1),"Some Port1");
		
		TS_ASSERT_THROWS(cameraList.getName(10), gphoto2pp::exceptions::gphoto2_exception); // out of bounds
	}
	
	void testFindByName()
	{
		gphoto2pp::CameraListWrapper cameraList;
		
		cameraList.append("First Model","Some Port1");
		cameraList.append("Second Model","Some Port2");
		
		TS_ASSERT_THROWS(cameraList.findByName("First"), gphoto2pp::exceptions::gphoto2_exception);
		
		auto index = cameraList.findByName("First Model");
		
		TS_ASSERT_EQUALS(index,0);
	}
	
	void testDuplicatesWithFindByName()
	{
		gphoto2pp::CameraListWrapper cameraList;
		
		cameraList.append("First Model","Some Port1");
		cameraList.append("First Model","Some Port1"); // Should I make the append method throw an error if the model aleady exists on the port? Best to ask Marcus
		
		//TS_ASSERT_EQUALS(cameraList.count(),1);
	}
	
	void testCopyAndAssignmentOperators()
	{
		gphoto2pp::CameraListWrapper cameraList;
		
		cameraList.append("First Model","Some Port1");
		cameraList.append("Second Model","Some Port2");
		
		{
			// Copy Constructor
			auto copy(cameraList);
			
			TS_ASSERT_EQUALS(cameraList.getName(0),"First Model");
			TS_ASSERT_EQUALS(copy.getName(0),"First Model");
		}
		
		{
			// Copy Assignment
			auto assignment = cameraList;
			
			TS_ASSERT_EQUALS(cameraList.getName(0),"First Model");
			TS_ASSERT_EQUALS(assignment.getName(0),"First Model");
		}
		
		{
			// Move Assignment
			auto moveassignment = std::move(cameraList);
			
			TS_ASSERT_EQUALS(moveassignment.getName(0),"First Model");
			TS_ASSERT_THROWS(cameraList.count(), gphoto2pp::exceptions::gphoto2_exception); // The old value is no longer valid
			
			cameraList = moveassignment; // now copy it back
			
			TS_ASSERT_EQUALS(cameraList.getName(0),"First Model");
		}
		
		{
			// Move Constructor
			auto moveconstructor(std::move(cameraList));
			
			TS_ASSERT_EQUALS(moveconstructor.getName(0),"First Model");
			TS_ASSERT_THROWS(cameraList.count(), gphoto2pp::exceptions::gphoto2_exception); // The old value is no longer valid again
			
			cameraList = moveconstructor; // now copy it back
			
			TS_ASSERT_EQUALS(cameraList.getName(0),"First Model");
		}
	}
};
