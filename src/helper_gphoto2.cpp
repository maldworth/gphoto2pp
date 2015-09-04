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

#include <gphoto2pp/helper_gphoto2.hpp>

#include <gphoto2pp/log.h>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>

#ifdef GPHOTO_LESS_25
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>
#include <gphoto2pp/camera_abilities_list_wrapper.hpp>
#else
#include <gphoto2pp/helper_context.hpp>
#endif

namespace gphoto2
{
#ifndef GPHOTO_LESS_25
#include <gphoto2/gphoto2-camera.h> // Needed for gp_camera_autodetect, but only used if gphoto is 2.5 or greater
#else
#include <gphoto2/gphoto2-result.h> // used for gp_result_as_string
#include <gphoto2/gphoto2-context.h>
#endif
#include <gphoto2/gphoto2-version.h>
}

#include <sstream>

namespace gphoto2pp
{
	std::pair<std::string, std::string> autoDetect()
	{
		auto cameraList = autoDetectAll();
		
		return cameraList.getPair(0);
	}

	CameraListWrapper autoDetectAll()
	{
		CameraListWrapper cameraListWrapper;
#ifdef GPHOTO_LESS_25
		//All this logic was added to the gp_camera_autodetect method in 2.5 or greater.
		
		// Constructor for this class will load all port drivers
		GPPortInfoListWrapper portInfoListWrapper;
		
		// Loads all the camera drivers we have
		CameraAbilitiesListWrapper cameraAbilitiesListWrapper;
		
		auto tempListWrapper = cameraAbilitiesListWrapper.listDetect(portInfoListWrapper);
		
		//Now we need to filter out the usb:
		for (int i = 0; i < tempListWrapper.count(); i++)
		{
			auto name = tempListWrapper.getName(i);
			auto value = tempListWrapper.getValue(i);
			
			if(!std::string("usb:").compare(value))
			{
				// We don't want to capture items with usb:
				continue;
			}
			cameraListWrapper.append(name, value);
		}
#else
		auto spContext = gphoto2pp::getContext();
		gphoto2pp::checkResponse(gphoto2::gp_camera_autodetect(cameraListWrapper.getPtr(), spContext.get()), "gp_camera_autodetect");
#endif
		int count = cameraListWrapper.count();
		
		if(count == 0)
		{
			throw exceptions::NoCameraFoundError("autoDetect(multiple)");
		}
		
		return std::move(cameraListWrapper);
	}
	
	int checkResponse(int result, std::string&& methodName)
	{
		if(result < 0)
		{
			std::stringstream errorMessage;
			errorMessage << methodName << ": failed with return code '" << result << "' and the reason is: '" << gphoto2::gp_result_as_string(result) << "'";
			FILE_LOG(logERROR) << "Exception Message - '"<< errorMessage.str().c_str() << "'";
			throw exceptions::gphoto2_exception(result, errorMessage.str());
		}
		return result;
	}

	int checkResponseSilent(int result, std::string&& methodName)
	{
		try
		{
			gphoto2pp::checkResponse(result, std::move(methodName));
		}
		catch(const exceptions::gphoto2_exception& e)
		{
			FILE_LOG(logERROR) << "Suppressed gphoto2_exception[" << e.what() <<"]";
		}
		catch(const std::runtime_error& e)
		{
			FILE_LOG(logCRITICAL) << "Suppressed runtime_error[" << e.what() <<"]";
		}
		catch(const std::exception& e)
		{
			FILE_LOG(logEMERGENCY) << "Suppressed std::exception";
		}
		return result;
	}

	std::string LibraryVersion(bool verbose)
	{
		gphoto2::GPVersionVerbosity verbosity = gphoto2::GP_VERSION_SHORT;
		if(verbose)
		{
			verbosity = gphoto2::GP_VERSION_VERBOSE;
		}
		
		// array of char pointers. Remember const char** is equivalent to char const**
		char const** resp = gphoto2::gp_library_version(verbosity);
		
		std::stringstream version;
		
		char const** iter = resp;
		
		for(iter = resp; *iter; iter++)
		{
			version << *iter << std::endl;
		}
		
		return version.str();
	}
}
