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

class Helpers_gphoto2_NoDevice : public CxxTest::TestSuite 
{	
public:
	void setUp()
	{
		FILELog::ReportingLevel() = logCRITICAL;
	}
	
	void testCheckResponseOk()
	{
		TS_ASSERT_EQUALS(gphoto2pp::checkResponse(0,"ok_method"), 0);
	}
	
	void testCheckResponseInvalid()
	{
		TS_ASSERT_THROWS(gphoto2pp::checkResponse(-1,"invalid_method"), gphoto2pp::exceptions::gphoto2_exception);
	}
	
	void testCheckResponseSilentOk()
	{
		TS_ASSERT_EQUALS(gphoto2pp::checkResponseSilent(0,"ok_method"), 0);
	}
	
	void testCheckResponseSilentInvalid()
	{
		TS_ASSERT_EQUALS(gphoto2pp::checkResponseSilent(-1,"invalid_method"), -1);
	}
	
	void testLibraryVersion()
	{
		auto version = gphoto2pp::LibraryVersion();
		
		TS_ASSERT(!version.empty());
	}
	
	void testCameraAutodetectSingle()
	{
		try
		{
			auto cameraDetails = gphoto2pp::autoDetect();
			
			// Frusterating, I must have an old version of cxxtest, as TS_SKIP is not declared
			//TS_SKIP("Camera was found, cannot complete this test");
		}
		catch(const gphoto2pp::exceptions::NoCameraFoundError& e)
		{
			TS_ASSERT(true);
		}
		catch(const std::exception& e)
		{
			TS_FAIL(e.what());
		}
	}
	
	void testCameraAutodetectMultiple()
	{
		try
		{
			auto cameraList = gphoto2pp::autoDetectAll();
			
			// Frusterating, I must have an old version of cxxtest, as TS_SKIP is not declared
			//TS_SKIP("Camera was found, cannot complete this test");
		}
		catch(const gphoto2pp::exceptions::NoCameraFoundError& e)
		{
			TS_ASSERT(true);
		}
		catch(const std::exception& e)
		{
			TS_FAIL(e.what());
		}
	}
};
