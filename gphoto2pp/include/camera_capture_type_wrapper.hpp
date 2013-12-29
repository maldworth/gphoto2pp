#ifndef CAMERACAPTURETYPEWRAPPER_HPP
#define CAMERACAPTURETYPEWRAPPER_HPP

namespace gphoto2pp
{
	/**
	 * Provides a C++ enum for the gphoto2 CameraCaptureType enum
	 */
	enum class CameraCaptureTypeWrapper : int
	{
		Image = 0,			///< Maps to GP_CAPTURE_IMAGE
		Movie = 1,			///< Maps to GP_CAPTURE_MOVIE
		Sound = 2,			///< Maps to GP_CAPTURE_SOUND
	};
}

#endif // CAMERACAPTURETYPEWRAPPER_HPP
