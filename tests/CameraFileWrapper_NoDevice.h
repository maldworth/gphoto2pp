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
#include <gphoto2pp/camera_file_path_wrapper.hpp>
#include <gphoto2pp/camera_capture_type_wrapper.hpp>
#include <gphoto2pp/camera_file_type_wrapper.hpp>
#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/log.h>

#include <fstream>

class CameraFileWrapper_NoDevice : public CxxTest::TestSuite 
{
	gphoto2pp::CameraFileWrapper _file;
	
	std::string fileName = std::string("unit_test_sample_output");
	std::string fileExtension = std::string(".someextension");
	
	std::string fullFileName = fileName + fileExtension;
	
public:
	void setUp()
	{
		FILELog::ReportingLevel() = logCRITICAL;
	}
	
	void testInitializeFile()
	{
		// Now we don't necessarily need to get a legitimate file (could read one from disk or use a dummy one), but I prefer to get a file from the camera to do all the tests.
		//~ gphoto2pp::CameraWrapper _camera;
		//~ auto filePath = _camera.capture(gphoto2pp::CameraCaptureTypeWrapper::Image);
		//~ _file = _camera.fileGet(filePath.Folder, filePath.Name, gphoto2pp::CameraFileTypeWrapper::Normal);
		//http://insanecoding.blogspot.ca/2011/11/how-to-read-in-file-in-c.html
		//http://stackoverflow.com/questions/441203/proper-way-to-store-binary-data-with-c-stl
		//http://stackoverflow.com/questions/347949/convert-stdstring-to-const-char-or-char
		//http://stackoverflow.com/questions/4761529/efficient-way-of-reading-a-file-into-an-stdvectorchar
		std::ifstream in("unit_test_sample_input.jpg", std::ios::in | std::ios::ate | std::ios::binary);
		if(in)
		{
			//~ std::vector<char> fileContents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()); // This is a shorter and cleaner way, but will resize the vector as it's made.
			std::vector<char> fileContents;
			fileContents.reserve(in.tellg());
			in.seekg(0, std::ios::beg);
			fileContents.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
			
			_file.setDataAndSize(fileContents);
		}
		else
		{
			TS_FAIL("Test file does not exist");
		}
	}
	
	void testSetFileName()
	{
		_file.setFileName(fullFileName);
	}
	
	void testMoveAssignment()
	{
		// Move Assignment
		auto moveassignment = std::move(_file);
		
		TS_ASSERT_EQUALS(moveassignment.getFileName(), fullFileName);
		
		TS_ASSERT_THROWS(_file.getFileName(), gphoto2pp::exceptions::gphoto2_exception);
		
		_file = std::move(moveassignment); // now assign it back
		
		TS_ASSERT_EQUALS(_file.getFileName(), fullFileName);
	}
	
	void testMoveConstructor()
	{
		// Move Constructor
		auto moveconstructor(std::move(_file));
		
		TS_ASSERT_EQUALS(moveconstructor.getFileName(), fullFileName);
		
		TS_ASSERT_THROWS(_file.getFileName(), gphoto2pp::exceptions::gphoto2_exception); // The old value is no longer valid again
		
		_file = std::move(moveconstructor); // now copy it back, ironically using move assignment which we previously checked that it was working
		
		TS_ASSERT_EQUALS(_file.getFileName(), fullFileName);
	}
	
	void testCopyAssignment()
	{
		// Copy Assignment
		{
			auto copyassignment = _file;
			
			TS_ASSERT_EQUALS(copyassignment.getFileName(), fullFileName);
			TS_ASSERT_EQUALS(_file.getFileName(), fullFileName);
		}
		
		// Now out of scope, the original file should still exist and work
		TS_ASSERT_EQUALS(_file.getFileName(), fullFileName);
	}
	
	void testCopyConstructor()
	{
		// Copy Constructor
		{
			auto copyconstructor(_file);
			
			TS_ASSERT_EQUALS(copyconstructor.getFileName(), fullFileName);
			TS_ASSERT_EQUALS(_file.getFileName(), fullFileName);
		}
		
		TS_ASSERT_EQUALS(_file.getFileName(), fullFileName);
	}
	
	void testMimeFunctions()
	{
		_file.detectMimeType(); // By default it will be unknown/unknown, this will now change it to hopefully image/png
		auto oldMimeType = _file.getMimeType(); // now with our sample file this should be image/jpeg
		
		TS_ASSERT_EQUALS(oldMimeType, "image/jpeg");
		
		_file.setMimeType("image/tiff");
		
		TS_ASSERT_EQUALS(_file.getMimeType(),"image/tiff");
		
		// Now we will adjust the mime type for our artifical mime type
		_file.adjustNameForMimeType();
		
		// It should now be 'unit_test.tif'
		TS_ASSERT_EQUALS(_file.getFileName(), fileName + ".tif");
		
		// Return the file to it's original Mime type
		// Originally I used setMimeType, however this is a perfect opportunity to test detectMimeType()
		//_file.setMimeType(oldMimeType);
		_file.detectMimeType();
		
		TS_ASSERT_EQUALS(_file.getMimeType(), oldMimeType);
	}
	
	void testTimeAccessors()
	{
		// I don't have any better way to test the time set and get for now. I just read, write it back with 5 seconds later, and then read and diff the two.
		
		auto read1 = _file.getMtime();
		
		_file.setMtime(std::time_t(read1 - 5));
		
		auto read2 = _file.getMtime();
		
		TS_ASSERT_EQUALS(std::difftime(read1, read2), 5);
	}
	
	void testSaveFile()
	{
		// Now we adjust the name to be the proper name, and we will save it with this name
		_file.adjustNameForMimeType();
		
		_file.save(_file.getFileName());
		
		std::ifstream in(_file.getFileName());
		
		TS_ASSERT(in.good());
	}
};
