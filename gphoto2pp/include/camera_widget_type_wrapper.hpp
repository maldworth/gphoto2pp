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

#ifndef WIDGETTYPE_HPP
#define WIDGETTYPE_HPP

namespace gphoto2pp
{
	/**
	 * Provides a C++ enum for the gphoto2 CameraWidgetType enum
	 */
	enum class CameraWidgetTypeWrapper : int
	{
		Window = 0,		///< Maps to GP_WIDGET_WINDOW
		Section = 1,	///< Maps to GP_WIDGET_SECTION
		Text = 2,		///< Maps to GP_WIDGET_TEXT
		Range = 3,		///< Maps to GP_WIDGET_RANGE
		Toggle = 4,		///< Maps to GP_WIDGET_TOGGLE
		Radio = 5,		///< Maps to GP_WIDGET_RADIO
		Menu = 6,		///< Maps to GP_WIDGET_MENU
		Button = 7,		///< Maps to GP_WIDGET_BUTTON
		Date = 8,		///< Maps to GP_WIDGET_DATE
	};
}


#endif // WIDGETTYPE_HPP
