#ifndef HELPERGPHOTO2_HPP
#define HELPERGPHOTO2_HPP

#include <string>
#include <vector>

namespace gphoto2
{
	struct _GPContext;
}


namespace gphoto2pp
{
	class CameraListWrapper;
	
	/**
	 * \brief Finds the first available and recognized camera type connected to the computer
	 * \return the model and port of the first camera found
	 * \throw GPhoto2pp::exceptions::NoCameraFoundError if it didn't find any cameras
	 */
	std::pair<std::string, std::string> autoDetect();

	/**
	 * \brief Finds all attached and recognized camera types connected to the computer
	 * \return a list of all cameras found
	 * \throw GPhoto2pp::exceptions::NoCameraFoundError if it didn't find any cameras
	 */
	CameraListWrapper autoDetectAll();
	
	/**
	 * \brief Helper used to validate the response of gphoto2 methods. when GP_ error codes are < 0, this method will create a gphoto2_exception
	 * \param[in]	result	of the gphoto2 method
	 * \param[in]	methodName	of the gphoto2 method that was called
	 * \return the integer result returned from all gphoto2 methods. All results < 0 will be caught and thrown in the exception type. Some gphoto2 methods return the count of items, and so values > 0 can be returned and unrelated to error codes
	 * \throw GPhoto2pp::exceptions::gphoto2_exception
	 */
	int checkResponse(int result, std::string&& methodName);
	
	/**
	 * \brief Helper used to validate the response of gphoto2 methods, but does not throw an exception (logs the error instead)
	 * \param[in]	result	of the gphoto2 method
	 * \param[in]	methodName	of the gphoto2 method that was called
	 * \return the integer result returned from all gphoto2 methods. This is usually <= 0 (gphoto2 error codes), but some methods return the count of items, and so values > 0 can be returned and unrelated to error codes
	 */
	int checkResponseSilent(int result, std::string&& methodName);

	/**
	 * \brief Returns the version of gphoto library currently linked in the system
	 * \param[in]	verbose	response, set to true, otherwise set to false
	 * \note Direct wrapper for gp_library_version(...)
	 */
	std::string LibraryVersion(bool verbose = false);
}

#endif // HELPERGPHOTO2_HPP
