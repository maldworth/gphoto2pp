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

#include "date_widget.hpp"

#include "exceptions.hpp"
#include "camera_widget_type_wrapper.hpp"

#include "log.h"

namespace gphoto2pp
{

	DateWidget::DateWidget(gphoto2::_CameraWidget* cameraWidget)
		: ValueWidgetBase(cameraWidget)
	{
		FILE_LOG(logINFO) << "DateWidget constructor - widget";	
		
		if(this->getType() != CameraWidgetTypeWrapper::Date)
		{
			throw exceptions::InvalidWidgetType("The widget type must be a Date Widget");
		}
	}
	
	std::time_t DateWidget::getValue() const
	{
		void* temp = this->getValueDefault();
		
		int temp2 = *reinterpret_cast<int*>(&temp);
		
		return static_cast<std::time_t>(temp2);
	}
	
	void DateWidget::setValue(const std::time_t& datetime)
	{
		const int temp = static_cast<int>(datetime);
		
		void* temp2 = const_cast<int*>(&temp);
		
		this->setValueDefault(temp2);
		
		//this->setValue(static_cast<int>(datetime));
	}
}

