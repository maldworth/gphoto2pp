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

#include <gphoto2pp/non_value_widget.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/exceptions.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{

	NonValueWidget::NonValueWidget(gphoto2::_CameraWidget* cameraWidget)
		: CameraWidgetWrapper{cameraWidget}
	{
	}
	
	int NonValueWidget::countChildren() const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_widget_count_children(m_cameraWidget),"gp_widget_count_children");
	}
	
	// Protected Methods, this call is done here so we don't have to put any gphoto includes in the header hpp files.
	gphoto2::_CameraWidget* NonValueWidget::getChildByNameWrapper(std::string const & name) const
	{
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child_by_name(m_cameraWidget, name.c_str(), &childWidget),"gp_widget_get_child_by_name");
		return childWidget;
	}
	
	gphoto2::_CameraWidget* NonValueWidget::getChildByLabelWrapper(std::string const & label) const
	{
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child_by_label(m_cameraWidget, label.c_str(), &childWidget),"gp_widget_get_child_by_label");
		return childWidget;
	}
	
	gphoto2::_CameraWidget* NonValueWidget::getChildWrapper(int index) const
	{
		if( index >= countChildren())
		{
			throw exceptions::IndexOutOfRange("You are trying to get a child widget at an index which is greater than the maximum index.");
		}
		
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child(m_cameraWidget, index, &childWidget),"gp_widget_get_child");
		return childWidget;
	}
	
	gphoto2::_CameraWidget* NonValueWidget::getChildByIdWrapper(int id) const
	{
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child_by_id(m_cameraWidget, id, &childWidget),"gp_widget_get_child_by_id");
		return childWidget;
	}
}

