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

#ifndef VALUEWIDGETBASE_HPP
#define VALUEWIDGETBASE_HPP

#include <gphoto2pp/camera_widget_wrapper.hpp>

namespace gphoto2pp
{
	/**
	 * \class ValueWidgetBase
	 * A templated class which must be inherited by any leaf widget type. That means any widget type that is not a Section or a Widget. That way their specific widget implementations can define meaningful Get and Set methods.
	 * \tparam the meaningful value type for the widget
	 */
	template<typename T>
	class ValueWidgetBase : public CameraWidgetWrapper
	{

	public:
		/**
		 * \brief Gets the widget's value in terms of T
		 * \return the widget's value
		 */
		virtual T getValue() const = 0;
		
		/**
		 * \brief Sets the widget's value in terms of T
		 * \param[in]	value	to set for the widget
		 * \return the widget's T value
		 */
		virtual void setValue(T const & value) = 0;
		
	protected:
		ValueWidgetBase(gphoto2::_CameraWidget* cameraWidget)
		: CameraWidgetWrapper{cameraWidget}
		{
			// Empty Constructor
		}
	};

}

#endif // VALUEWIDGETBASE_HPP
