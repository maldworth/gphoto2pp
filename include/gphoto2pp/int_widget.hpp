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

#ifndef INTWIDGET_HPP
#define INTWIDGET_HPP

#include <gphoto2pp/value_widget_base.hpp>

namespace gphoto2pp
{
	/**
	 * \class IntWidget
	 * A class representing gphoto2 widgets which have a value that is meaningfully represented by an int
	 */
	class IntWidget: public ValueWidgetBase<int>
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's value in terms of int
		 * \return the widget's int value
		 * \note Direct wrapper for <tt>gp_widget_get_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getValue() const override;
		
		/**
		 * \brief Sets the widget's value in terms of int
		 * \param[in]	value	to set the widget to
		 * \note Direct wrapper for <tt>gp_widget_set_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValue(int const & value) override;
		
	protected:
		IntWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // INTWIDGET_HPP
