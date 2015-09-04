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

class Helpers_gphoto2_Generic : public CxxTest::TestSuite 
{
	std::pair<std::string, std::string> _cameraDetails;
	
public:
	void setUp()
	{
		FILELog::ReportingLevel() = logCRITICAL;
	}
	
	void testCameraAutodetectSingle()
	{
		try
		{
			_cameraDetails = gphoto2pp::autoDetect();
		}
		catch(const gphoto2pp::exceptions::NoCameraFoundError& e)
		{
			TS_FAIL("No Camera detected, please make sure your camera is on and plugged in");
		}
		catch(const std::exception& e)
		{
			TS_FAIL(e.what());
		}
	}
	
	void testCameraAutodetectMultiple()
	{
		std::vector<std::string> models;
		std::vector<std::string> ports;
		try
		{
			auto cameraList = gphoto2pp::autoDetectAll();
			
			if(cameraList.count() <= 1)
			{
				// No TS_SKIP available :(
				//TS_SKIP("Only 1 Camera Found, cannot complete this test, skipping...");
			}
		}
		catch(const gphoto2pp::exceptions::NoCameraFoundError& e)
		{
			TS_FAIL("No Camera detected, please make sure your camera is on and plugged in");
		}
		catch(const std::exception& e)
		{
			TS_FAIL(e.what());
		}
	}
	
	
};
