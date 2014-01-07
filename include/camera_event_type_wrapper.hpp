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

#ifndef CAMERAEVENTTYPEWRAPPER_HPP
#define CAMERAEVENTTYPEWRAPPER_HPP

namespace gphoto2pp
{
	/**
	 * Provides a C++ enum for the gphoto2 CameraEventType enum
	 */
	enum class CameraEventTypeWrapper : int
	{
		Unknown = 0,			///< Maps to GP_EVENT_UNKNOWN
		Timeout = 1,			///< Maps to GP_EVENT_TIMEOUT
		FileAdded = 2,			///< Maps to GP_EVENT_FILE_ADDED
		FolderAdded = 3,		///< Maps to GP_EVENT_FOLDER_ADDED
		CaptureComplete = 4,	///< Maps to GP_EVENT_CAPTURE_COMPLETE
	};
}

#endif // CAMERAEVENTTYPEWRAPPER_HPP
