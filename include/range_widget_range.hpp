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

#ifndef RANGEWIDGETRANGE_HPP
#define RANGEWIDGETRANGE_HPP

#include <string>

namespace gphoto2pp
{
	/** struct RangeWidgetRange
	 * Provides a POD for the RangeWidget's getRange method.
	 */
	struct RangeWidgetRange
	{
		float Min;
		float Max;
		float Step;
	};
}

#endif // RANGEWIDGETRANGE_HPP
