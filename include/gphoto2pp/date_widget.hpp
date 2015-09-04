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

#ifndef DATEWIDGET_HPP
#define DATEWIDGET_HPP

#include <gphoto2pp/value_widget_base.hpp>

#include <ctime>

namespace gphoto2pp
{
	/**
	 * \class DateWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Date
	 */
	class DateWidget: public ValueWidgetBase<std::time_t>
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's value in terms of std::time_t
		 * \return the widget's std::time_t value
		 * \note Direct wrapper for <tt>gp_widget_get_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::time_t getValue() const override;
		
		/**
		 * \brief Sets the widget's value in terms of std::time_t
		 * \param[in]	date	to set for the widget
		 * \note Direct wrapper for gp_widget_set_value(...)
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValue(std::time_t const & date) override;

	protected:
		DateWidget(gphoto2::_CameraWidget* cameraWidget);
	};
}

#endif // DATEWIDGET_HPP
