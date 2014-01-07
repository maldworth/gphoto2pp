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

#ifndef CAMERAFILETYPEWRAPPER_HPP
#define CAMERAFILETYPEWRAPPER_HPP

namespace gphoto2pp
{
	/**
	 * Provides a C++ enum for the gphoto2 CameraFileType enum
	 */
	enum class CameraFileTypeWrapper : int
	{
		Preview = 0,	///< Maps to GP_FILE_TYPE_PREVIEW
		Normal = 1,		///< Maps to GP_FILE_TYPE_NORMAL
		Raw = 2,		///< Maps to GP_FILE_TYPE_RAW
		Audio = 3,		///< Maps to GP_FILE_TYPE_AUDIO
		Exif = 4,		///< Maps to GP_FILE_TYPE_EXIF
		MetaData = 5	///< Maps to GP_FILE_TYPE_METADATA
	};
	
}


#endif // CAMERAFILETYPEWRAPPER_HPP
