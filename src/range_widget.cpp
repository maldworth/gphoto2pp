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

#include <gphoto2pp/range_widget.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/range_widget_range.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

#include <sstream>

namespace gphoto2pp
{

	RangeWidget::RangeWidget(gphoto2::_CameraWidget* cameraWidget)
		: FloatWidget{cameraWidget}
	{
		if(this->getType() != CameraWidgetTypeWrapper::Range)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Range Widget");
		}
	}
	
	RangeWidgetRange RangeWidget::getRange() const
	{
		auto range = RangeWidgetRange{0,0,0};
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_range(m_cameraWidget, &range.Min, &range.Max, &range.Step),"gp_widget_get_range");
		return range;
	}
	
	std::string RangeWidget::ToString() const
	{
		float m_min = 0, m_max = 0, m_step = 0;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_range(m_cameraWidget, &m_min, &m_max, &m_step),"gp_widget_get_range");
		
		std::stringstream temp;
		temp << "[min: " << m_min << " max: " << m_max << " step: " << m_step << "]";
		return temp.str();
	}

}

