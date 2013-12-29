#include <cxxtest/TestSuite.h>

#include "helper_gphoto2.hpp"
#include "camera_list_wrapper.hpp"
#include "exceptions.hpp"
#include "log.h"

class CameraListWrapper_NoDevice : public CxxTest::TestSuite 
{
public:
	void setUp()
	{
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logCRITICAL;
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
