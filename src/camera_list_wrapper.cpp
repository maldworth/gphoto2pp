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

#include <gphoto2pp/camera_list_wrapper.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2-list.h>
}

namespace gphoto2pp
{

	CameraListWrapper::CameraListWrapper()
		: m_cameraList{nullptr}
	{
		gphoto2pp::checkResponse(gphoto2::gp_list_new(&m_cameraList),"gp_list_new");
	}

	CameraListWrapper::~CameraListWrapper()
	{
		if(m_cameraList != nullptr) // Added this line so the move constructor wouldn't dispose of the object if it was reassigned
		{
			gphoto2pp::checkResponseSilent(gphoto2::gp_list_unref(m_cameraList),"gp_list_unref");
			m_cameraList = nullptr;
		}
	}
	
	CameraListWrapper::CameraListWrapper(CameraListWrapper&& other)
		: m_cameraList{other.m_cameraList}
	{
		other.m_cameraList = nullptr;
	}
	
	CameraListWrapper& CameraListWrapper::operator=(CameraListWrapper&& other)
	{
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraList != nullptr)
			{
				gphoto2pp::checkResponse(gphoto2::gp_list_unref(m_cameraList),"gp_list_unref");
			}
			
			// Steal or "move" the other objects resource
			m_cameraList = other.m_cameraList;
			
			// Unreference the other objects resource, so it's destructor doesn't unreference it
			other.m_cameraList = nullptr;
		}
		return *this;
	}
	
	CameraListWrapper::CameraListWrapper(CameraListWrapper const & other)
		: m_cameraList{other.m_cameraList}
	{
		// Because we now refer to the same cameralist as "other", we need to add to it's reference count
		if(m_cameraList != nullptr)
		{
			gphoto2::gp_list_ref(m_cameraList);
		}
	}
	
	CameraListWrapper& CameraListWrapper::operator=(CameraListWrapper const & other)
	{
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraList != nullptr)
			{
				gphoto2pp::checkResponse(gphoto2::gp_list_unref(m_cameraList),"gp_list_unref");
			}
			
			// copy the other objects pointer
			m_cameraList = other.m_cameraList;
			
			// Add reference count to the copied cameraList
			if(m_cameraList != nullptr)
			{
				gphoto2pp::checkResponse(gphoto2::gp_list_ref(m_cameraList),"gp_list_ref");
			}
		}
		return *this;
	}
	
	gphoto2::_CameraList* CameraListWrapper::getPtr() const
	{
		return m_cameraList;
	}

	int CameraListWrapper::count() const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_list_count(m_cameraList),"gp_list_count");
	}

	std::string CameraListWrapper::getName(int index) const
	{
		const char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_list_get_name(m_cameraList, index, &temp),"gp_list_get_name");
		
		if(temp == nullptr)
		{
			return std::string{};
		}
		else
		{
			return std::string{temp};
		}
	}

	std::string CameraListWrapper::getValue(int index) const
	{
		char const * temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_list_get_value(m_cameraList, index, &temp),"gp_list_get_value");
		
		if(temp == nullptr) // sometimes it returns null, so we will just consider that an empty string
		{
			return std::string{};
		}
		else
		{
			return std::string{temp};
		}
	}
	
	void CameraListWrapper::append(std::string const & name, std::string const & value)
	{
		gphoto2pp::checkResponse(gphoto2::gp_list_append(m_cameraList, name.c_str(), value.c_str()),"gp_list_append");
	}
	
	void CameraListWrapper::reset()
	{
		gphoto2pp::checkResponse(gphoto2::gp_list_reset(m_cameraList),"gp_list_reset");
	}
	
	void CameraListWrapper::sort()
	{
		gphoto2pp::checkResponse(gphoto2::gp_list_sort(m_cameraList),"gp_list_sort");
	}
	
	int CameraListWrapper::findByName(std::string const & name) const
	{
		int index;
		
		gphoto2pp::checkResponse(gphoto2::gp_list_find_by_name(m_cameraList, &index, name.c_str()),"gp_list_find_by_name");
		
		return index;
	}
	
	void CameraListWrapper::setName(int index, std::string const & name)
	{
		gphoto2pp::checkResponse(gphoto2::gp_list_set_name(m_cameraList, index, name.c_str()),"gp_list_set_name");
	}

	void CameraListWrapper::setValue(int index, std::string const & value)
	{
		gphoto2pp::checkResponse(gphoto2::gp_list_set_value(m_cameraList, index, value.c_str()),"gp_list_set_value");
	}
	
	std::pair<std::string, std::string> CameraListWrapper::getPair(int index) const
	{
		return std::make_pair(getName(index), getValue(index));
	}
		
	std::pair<std::string, std::string> CameraListWrapper::getPairByName(std::string const & name) const
	{
		auto index = findByName(name);
		
		return std::make_pair(getName(index), getValue(index));
	}
}

