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

#include "string_widget.hpp"

#include "helper_gphoto2.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "exceptions.hpp"

#include "log.h"

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{

	StringWidget::StringWidget(gphoto2::_CameraWidget* cameraWidget)
		: ValueWidgetBase(cameraWidget)
	{
		FILE_LOG(logINFO) << "StringWidget constructor - widget";
	}

	std::string StringWidget::getValue() const
	{
		char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_value(m_cameraWidget, &temp),"gp_widget_get_value");
		
		return std::string(temp);
	}
	
	void StringWidget::setValue(const std::string& value)
	{
		gphoto2pp::checkResponse(gphoto2::gp_widget_set_value(m_cameraWidget, value.c_str()),"gp_widget_set_value");
	}

}

