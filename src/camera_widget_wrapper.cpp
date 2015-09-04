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

#include <gphoto2pp/camera_widget_wrapper.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{
	CameraWidgetWrapper::CameraWidgetWrapper(gphoto2::_CameraWidget* cameraWidget)
		: m_cameraWidget{cameraWidget}
	{
		// This call to increment the reference is correct 99% of the time when this constructor is called
		// because it is likely being called for a child widget. However the one scenario when we call this
		// constructor from the CameraWrapper.getConfig, it is the parent, and so we have now just incremented
		// the reference to == 2. This is wrong, and so we unref right after in CameraWrapper::getConfig
		ref();
	}
	
	CameraWidgetWrapper::~CameraWidgetWrapper()
	{
		if(m_cameraWidget != nullptr)
		{	
			unref();
			
			m_cameraWidget = nullptr;
		}
	}

	CameraWidgetWrapper::CameraWidgetWrapper(CameraWidgetWrapper&& other)
		: m_cameraWidget{other.m_cameraWidget}
	{
		other.m_cameraWidget = nullptr; // So when the 'other' instance calls it's destructor, it won't try to unreference.
	}

	CameraWidgetWrapper& CameraWidgetWrapper::operator=(CameraWidgetWrapper&& other)
	{
		// Check for self assignment
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraWidget != nullptr)
			{
				unref();
			}
			
			// Steal or "move" the other objects resource
			m_cameraWidget = other.m_cameraWidget;
			
			// Unreference the other objects resource, so it's destructor doesn't unreference it
			other.m_cameraWidget = nullptr;
		}
		
		return *this;
	}
	
	CameraWidgetWrapper::CameraWidgetWrapper(CameraWidgetWrapper const & other)
		: m_cameraWidget{other.m_cameraWidget}
	{
		ref();
	}

	CameraWidgetWrapper& CameraWidgetWrapper::operator=(CameraWidgetWrapper const & other)
	{
		// Check for self assignment
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraWidget != nullptr)
			{
				unref();
			}
			
			// copy the other objects pointer
			m_cameraWidget = other.m_cameraWidget;
			
			// Now we add reference to thew new one we just copied
			if(m_cameraWidget != nullptr)
			{
				ref();
			}
		}
		return *this;
	}
	
	gphoto2::_CameraWidget* CameraWidgetWrapper::getPtr() const
	{
		return m_cameraWidget;
	}

	std::string CameraWidgetWrapper::getName() const
	{
		const char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_name(m_cameraWidget, &temp),"gp_widget_get_name");
		
		return std::string(temp);
	}

	CameraWidgetTypeWrapper CameraWidgetWrapper::getType() const
	{
		gphoto2::CameraWidgetType temp;

		gphoto2pp::checkResponse(gphoto2::gp_widget_get_type(m_cameraWidget, &temp),"gp_widget_get_type");
		
		return static_cast<CameraWidgetTypeWrapper>(temp);
	}

	std::string CameraWidgetWrapper::getLabel() const
	{
		const char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_label(m_cameraWidget, &temp),"gp_widget_get_label");
		
		return std::string(temp);
	}

	std::string CameraWidgetWrapper::getInfo() const
	{
		const char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_info(m_cameraWidget, &temp),"gp_widget_get_info");
		
		return std::string(temp);
	}
	
	int CameraWidgetWrapper::getId() const
	{
		int id = 0;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_id(m_cameraWidget, &id),"gp_widget_get_id");
		
		return id;
	}
	
	CameraWidgetWrapper CameraWidgetWrapper::getRoot() const
	{
		return CameraWidgetWrapper(getRootDefault());
	}
	
	CameraWidgetWrapper CameraWidgetWrapper::getParent() const
	{
		return CameraWidgetWrapper(getParentDefault());
	}
	
	gphoto2::_CameraWidget* CameraWidgetWrapper::getRootDefault() const
	{
		gphoto2::_CameraWidget* rootWidget = nullptr;

		gphoto2pp::checkResponse(gphoto2::gp_widget_get_root(m_cameraWidget, &rootWidget),"gp_widget_get_root");
		
		return rootWidget;
	}
	
	void CameraWidgetWrapper::ref()
	{
		gphoto2::_CameraWidget* rootWidget = getRootDefault();
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_ref(rootWidget),"gp_widget_ref");
	}
	
	void CameraWidgetWrapper::unref()
	{
		gphoto2::_CameraWidget* rootWidget = getRootDefault();
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_unref(rootWidget),"gp_widget_unref");
	}
	
	gphoto2::_CameraWidget* CameraWidgetWrapper::getParentDefault() const
	{
		gphoto2::_CameraWidget* parentWidget = nullptr;

		gphoto2pp::checkResponse(gphoto2::gp_widget_get_parent(m_cameraWidget, &parentWidget),"gp_widget_get_parent");
		
		return parentWidget;
	}
}

