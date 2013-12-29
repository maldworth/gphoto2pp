#include <cxxtest/TestSuite.h>

#include "helper_gphoto2.hpp"
#include "helper_debugging.hpp"
#include "camera_wrapper.hpp"
#include "camera_file_path_wrapper.hpp"
#include "camera_list_wrapper.hpp"
#include "exceptions.hpp"
#include "camera_capture_type_wrapper.hpp"
#include "window_widget.hpp"
#include "log.h"

class CameraWrapper_Generic : public CxxTest::TestSuite 
{
	gphoto2pp::CameraWrapper _camera;
	gphoto2pp::CameraFilePathWrapper _captureFilePath;
	
public:
	void setUp()
	{
		gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logCRITICAL;
	}
	
	void testSummary()
	{
		TS_ASSERT(!_camera.getSummary().empty());
	}
	
	void testMoveAssignment()
	{
		// Move Assignment
		auto moveassignment = std::move(_camera);
		
		TS_ASSERT(!moveassignment.getSummary().empty());
		TS_ASSERT_THROWS(_camera.getSummary(), gphoto2pp::exceptions::gphoto2_exception);
		
		_camera = std::move(moveassignment); // now assign it back
		
		TS_ASSERT(!_camera.getSummary().empty());
	}
	
	void testMoveConstructor()
	{
		// Move Constructor
		auto moveconstructor(std::move(_camera));
		
		TS_ASSERT(!moveconstructor.getSummary().empty());
		TS_ASSERT_THROWS(_camera.getSummary(), gphoto2pp::exceptions::gphoto2_exception); // The old value is no longer valid again
		
		_camera = std::move(moveconstructor); // now assign it back, ironically using move assignment which we previously checked that it was working
		
		TS_ASSERT(!_camera.getSummary().empty());
	}
	
	void testCapture()
	{
		_captureFilePath = _camera.capture(gphoto2pp::CameraCaptureTypeWrapper::Image);
	}
	
	void testSDCardAccessingAndDeletingFiles()
	{
		// The previous test should have taken a picture and put the temporary image in the root folder "/"
		
		auto files = _camera.folderListFiles(_captureFilePath.Folder);
		
		// Makes sure we have files
		TS_ASSERT_LESS_THAN(0, files.count());
		
		TS_ASSERT(files.getName(0).find(_captureFilePath.Name) != std::string::npos);
		
		_camera.fileDelete(_captureFilePath.Folder,_captureFilePath.Name);
		
		//Now lets delete the file when it's not there, this should throw an exception
		TS_ASSERT_THROWS(_camera.fileDelete(_captureFilePath.Folder,_captureFilePath.Name);, gphoto2pp::exceptions::gphoto2_exception);
		
		//Now we know there will be no files
		files = _camera.folderListFiles(_captureFilePath.Folder);
		
		TS_ASSERT_EQUALS(files.count(),0);
	}
	
	void testSDCardAccessingFolders()
	{
		auto folders = _camera.folderListFolders("/");
		
		// Makes sure we have at least one sub folder, which we should at the root of the camera filesystem
		TS_ASSERT_LESS_THAN(0, folders.count());
		
		auto rootFolder = "/"+folders.getName(0)+"/";
		
		TS_ASSERT(!rootFolder.empty());
	}
	
	void testGetAndSetConfig()
	{
		auto config = _camera.getConfig();
		
		TS_ASSERT(!config.getName().empty());
		
		_camera.setConfig(config);
	}
};
