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

#include <gphoto2pp/camera_abilities_list_wrapper.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/helper_context.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

#include <gphoto2pp/log.h>

namespace gphoto2
{
#include <gphoto2/gphoto2-abilities-list.h>
}

namespace gphoto2pp
{

	CameraAbilitiesListWrapper::CameraAbilitiesListWrapper()
		: m_cameraAbilitiesList{nullptr}
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper Constructor";
		
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_new(&m_cameraAbilitiesList),"gp_abilities_list_new");
		
		auto spContext = gphoto2pp::getContext();
		
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_load(m_cameraAbilitiesList, spContext.get()),"gp_abilities_list_load");
	}

	CameraAbilitiesListWrapper::~CameraAbilitiesListWrapper()
	{
		FILE_LOG(logINFO) << "~CameraAbilitiesListWrapper Destructor";
		
		if(m_cameraAbilitiesList != nullptr)
		{
			gphoto2pp::checkResponseSilent(gphoto2::gp_abilities_list_free(m_cameraAbilitiesList),"gp_abilities_list_free");
			m_cameraAbilitiesList = nullptr;
		}	
	}
	
	CameraAbilitiesListWrapper::CameraAbilitiesListWrapper(CameraAbilitiesListWrapper&& other)
		: m_cameraAbilitiesList{other.m_cameraAbilitiesList}
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move Constructor";
		
		other.m_cameraAbilitiesList = nullptr;
	}
	
	CameraAbilitiesListWrapper& CameraAbilitiesListWrapper::operator=(CameraAbilitiesListWrapper&& other)
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move assignment operator";
		
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraAbilitiesList != nullptr)
			{
				FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move assignment - current abilities is not null";
				gphoto2pp::checkResponse(gphoto2::gp_abilities_list_free(m_cameraAbilitiesList),"gp_abilities_list_free");
			}
			
			// Steal or "move" the other objects resource
			m_cameraAbilitiesList = other.m_cameraAbilitiesList;
			
			// Unreference the other objects resource, so it's destructor doesn't unreference it
			other.m_cameraAbilitiesList = nullptr;
		}
		return *this;
	}
	
	gphoto2::_CameraAbilitiesList* CameraAbilitiesListWrapper::getPtr() const
	{
		return m_cameraAbilitiesList;
	}

	CameraListWrapper CameraAbilitiesListWrapper::listDetect(GPPortInfoListWrapper const & portInfoList)
	{
		auto spContext = gphoto2pp::getContext();
		CameraListWrapper cameraList;
		
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_detect(m_cameraAbilitiesList, portInfoList.getPtr(), cameraList.getPtr(), spContext.get()),"gp_abilities_list_detect");
		
		return cameraList; // Compiler should do RVO here
	}

	void CameraAbilitiesListWrapper::reset()
	{
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_reset(m_cameraAbilitiesList),"gp_abilities_list_reset");
	}

	int CameraAbilitiesListWrapper::count() const
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper count";
		
		return gphoto2pp::checkResponse(gphoto2::gp_abilities_list_count(m_cameraAbilitiesList),"gp_abilities_list_count");
	}

	// Meant to return the index of the specified model, or gphoto error code
	int CameraAbilitiesListWrapper::lookupModel(std::string const & model) const
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper lookupModel - model";
		
		return gphoto2pp::checkResponse(gphoto2::gp_abilities_list_lookup_model(m_cameraAbilitiesList, model.c_str()),"gp_abilities_list_lookup_model");
	}

}

