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

#include <gphoto2pp/helper_camera_wrapper.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/log.h>

class Helpers_gphoto2_Generic : public CxxTest::TestSuite 
{
	gphoto2pp::CameraWrapper _camera;
	
public:
	void setUp()
	{
		FILELog::ReportingLevel() = logCRITICAL;
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
