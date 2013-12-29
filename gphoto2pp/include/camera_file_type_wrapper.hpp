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
