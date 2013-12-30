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

#ifndef CAMERAFILEPATHWRAPPER_HPP
#define CAMERAFILEPATHWRAPPER_HPP

#include <string>

namespace gphoto2pp
{
	/** struct CameraFilePathWrapper
	 * Provides a POD for the camerafilepath. This doesn't actually wrap the gphoto2 CameraFilePath struct because the structure was so simple
	 */
	struct CameraFilePathWrapper
	{
		std::string Name;
		std::string Folder;
	};
}

#endif // CAMERAFILEPATHWRAPPER_HPP
