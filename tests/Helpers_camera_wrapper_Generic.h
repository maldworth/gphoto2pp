#include <cxxtest/TestSuite.h>

#include "helper_camera_wrapper.hpp"
#include "camera_wrapper.hpp"
#include "camera_file_wrapper.hpp"
#include "camera_list_wrapper.hpp"
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
	
	void testCaptureAutoDelete()
	{
		gphoto2pp::CameraFileWrapper cameraFile;
		TS_ASSERT_THROWS_NOTHING(gphoto2pp::helper::capture(_camera, cameraFile, true));
		
		// gphoto2 < 2.5 stores temp files here
		auto files_loc1 = _camera.folderListFiles("/store_00010001");
		
		// gphoto2 >= 2.5 stores temp files here
		auto files_loc2 = _camera.folderListFiles("/");
		
		TS_ASSERT_EQUALS(files_loc1.count(), 0);
		TS_ASSERT_EQUALS(files_loc2.count(), 0);
		
		//TODO, save the file to hard disk and check for it's presence
	}
	
	void testCaptureNoAutoDelete()
	{
		gphoto2pp::CameraFileWrapper cameraFile;
		TS_ASSERT_THROWS_NOTHING(gphoto2pp::helper::capture(_camera, cameraFile, false));
		
		// gphoto2 < 2.5 stores temp files here
		auto files_loc1 = _camera.folderListFiles("/store_00010001");
		
		// gphoto2 >= 2.5 stores temp files here
		auto files_loc2 = _camera.folderListFiles("/");
		
		TS_ASSERT(files_loc1.count() == 1 || files_loc2.count() == 1);
	}
};
