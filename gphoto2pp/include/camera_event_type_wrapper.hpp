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
