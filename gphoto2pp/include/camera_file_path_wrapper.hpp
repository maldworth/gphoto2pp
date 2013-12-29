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
