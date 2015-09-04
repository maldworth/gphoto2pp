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

#ifndef RANGEWIDGET_HPP
#define RANGEWIDGET_HPP

#include <gphoto2pp/float_widget.hpp>

namespace gphoto2pp
{
	struct RangeWidgetRange;
	
	/**
	 * \class RangeWidget
	 * A class representing gphoto2 widgets which are of the widget type GPhoto2pp::CameraWidgetTypeWrapper::Range
	 */
	class RangeWidget: public FloatWidget
	{
	friend class NonValueWidget;

	public:
		/**
		 * \brief Gets the widget's range.
		 * \return the widget's range
		 * \note Direct wrapper for <tt>gp_widget_get_range(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		RangeWidgetRange getRange() const;
		
		/**
		 * \brief Gets the widget's range and returns their values in a user friendly string.
		 * \return the widget's range
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string ToString() const;

	protected:
		RangeWidget(gphoto2::_CameraWidget* cameraWidget);
	};

}

#endif // RANGEWIDGET_HPP
