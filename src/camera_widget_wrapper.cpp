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
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/window_widget.hpp>
#include <gphoto2pp/section_widget.hpp>
#include <gphoto2pp/text_widget.hpp>
#include <gphoto2pp/range_widget.hpp>
#include <gphoto2pp/int_widget.hpp>
#include <gphoto2pp/toggle_widget.hpp>
#include <gphoto2pp/radio_widget.hpp>
#include <gphoto2pp/menu_widget.hpp>
#include <gphoto2pp/button_widget.hpp>
#include <gphoto2pp/date_widget.hpp>
#include "widget_string_helper.hpp"

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{
	std::unique_ptr<CameraWidgetWrapper> CameraWidgetWrapper::create(
		CameraWidgetTypeWrapper type, std::string const & label)
	{
		if (label.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera widget label must not contain embedded NUL");
		}
		gphoto2::_CameraWidget *raw = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_widget_new(
			static_cast<gphoto2::CameraWidgetType>(type),
			label.c_str(), &raw), "gp_widget_new");
		if (raw == nullptr)
		{
			throw exceptions::NullWidget(
				"libgphoto2 returned a null camera widget");
		}
		std::unique_ptr<CameraWidgetWrapper> result;
		switch (type)
		{
			case CameraWidgetTypeWrapper::Window:
				result.reset(new WindowWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Section:
				result.reset(new SectionWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Text:
				result.reset(new TextWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Range:
				result.reset(new RangeWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Toggle:
				result.reset(new ToggleWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Radio:
				result.reset(new RadioWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Menu:
				result.reset(new MenuWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Button:
				result.reset(new ButtonWidget(raw));
				break;
			case CameraWidgetTypeWrapper::Date:
				result.reset(new DateWidget(raw));
				break;
			default:
				gphoto2::gp_widget_unref(raw);
				throw exceptions::InvalidWidgetType(
					"Unsupported standalone camera widget type");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_unref(raw), "gp_widget_unref");
		return result;
	}

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

	void CameraWidgetWrapper::releaseOwnership()
	{
		m_cameraWidget = nullptr;
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
		if (temp == nullptr)
		{
			throw exceptions::NullWidget("Camera widget name is null");
		}
		
		return std::string(temp);
	}

	std::string CameraWidgetWrapper::getName(std::size_t maxLength) const
	{
		const char* temp = nullptr;
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_get_name(m_cameraWidget, &temp),
			"gp_widget_get_name");
		return detail::copyWidgetString(temp, maxLength, "name");
	}

	void CameraWidgetWrapper::setName(std::string const & name)
	{
		if (name.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera widget name must not contain embedded NUL");
		}
		if (name.size() >= 256)
		{
			throw exceptions::ArgumentException(
				"Camera widget name exceeds libgphoto2 fixed limit");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_set_name(m_cameraWidget, name.c_str()),
			"gp_widget_set_name");
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
		if (temp == nullptr)
		{
			throw exceptions::NullWidget("Camera widget label is null");
		}
		
		return std::string(temp);
	}

	std::string CameraWidgetWrapper::getLabel(std::size_t maxLength) const
	{
		const char* temp = nullptr;
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_get_label(m_cameraWidget, &temp),
			"gp_widget_get_label");
		return detail::copyWidgetString(temp, maxLength, "label");
	}

	std::string CameraWidgetWrapper::getInfo() const
	{
		const char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_info(m_cameraWidget, &temp),"gp_widget_get_info");
		if (temp == nullptr)
		{
			throw exceptions::NullWidget("Camera widget info is null");
		}
		
		return std::string(temp);
	}

	void CameraWidgetWrapper::setInfo(std::string const & info)
	{
		if (info.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera widget info must not contain embedded NUL");
		}
		if (info.size() >= 1024)
		{
			throw exceptions::ArgumentException(
				"Camera widget info exceeds libgphoto2 fixed limit");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_set_info(m_cameraWidget, info.c_str()),
			"gp_widget_set_info");
	}
	
	int CameraWidgetWrapper::getId() const
	{
		int id = 0;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_id(m_cameraWidget, &id),"gp_widget_get_id");
		
		return id;
	}

	int CameraWidgetWrapper::getReadOnly() const
	{
		int readOnly = 0;

		gphoto2pp::checkResponse(
			gphoto2::gp_widget_get_readonly(m_cameraWidget, &readOnly),
			"gp_widget_get_readonly");

		return readOnly;
	}

	void CameraWidgetWrapper::setReadOnly(int readOnly)
	{
		if (readOnly != 0 && readOnly != 1)
		{
			throw exceptions::ArgumentException(
				"Camera widget read-only flag must be 0 or 1");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_set_readonly(m_cameraWidget, readOnly),
			"gp_widget_set_readonly");
	}

	bool CameraWidgetWrapper::isChanged() const
	{
		const int changed = gphoto2pp::checkResponse(
			gphoto2::gp_widget_changed(m_cameraWidget),
			"gp_widget_changed");
		return changed != 0;
	}

	void CameraWidgetWrapper::setChanged(int changed)
	{
		if (changed != 0 && changed != 1)
		{
			throw exceptions::ArgumentException(
				"Camera widget changed state must be 0 or 1");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_widget_set_changed(m_cameraWidget, changed),
			"gp_widget_set_changed");
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

