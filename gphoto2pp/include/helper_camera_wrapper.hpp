#ifndef HELPERCAMERAWRAPPER_HPP
#define HELPERCAMERAWRAPPER_HPP

#include "camera_file_type_wrapper.hpp"
#include "camera_capture_type_wrapper.hpp"

#include <string>
#include <vector>

namespace gphoto2pp
{
	// Forward Declarations
	class CameraWrapper;
	class CameraFileWrapper;
	
	namespace helper
	{
		/**
		 * \brief Takes a preview picture from the camera.
		 * This capture type might not be supported by all cameras (requires a live view/mirror lockup mode for continuous captures)
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture_preview command
		 * \param[in]	outputFilename	will be used when saving the captured image to disk
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void capturePreview(CameraWrapper& cameraWrapper, const std::string& outputFilename);
		
		/**
		 * \brief Takes a preview picture from the camera.
		 * This capture type might not be supported by all cameras (requires a live view/mirror lockup mode for continuous captures)
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture_preview command
		 * \param[out]	outputStream	will contain the file data in the ostream memory
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void capturePreview(CameraWrapper& cameraWrapper, std::ostream& outputStream);
		
		/**
		 * \brief Captures a file from the camera.
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \param[out]	cameraFile	will have the captured file's contents loaded into it's instance
		 * \param[in]	autoDeleteImageFromSrc	will remove the file from temporary memory after it's contents have been copied into the cameraFile. The capt0000 is never cleaned up automatically until the camera exists. Most cases will want this set to true, especially long running programs.
		 * \param[in]	captureType	indicates to the camera what file we want (Image, Sound, Movie)
		 * \param[in]	fileType	indicates the type of file to capture from the camera (Raw, Normal, exif, etc...). Most modern cameras can leave this as default.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void capture(CameraWrapper& cameraWrapper, CameraFileWrapper& cameraFile, bool autoDeleteFileFromSrc = false, const CameraCaptureTypeWrapper& captureType = CameraCaptureTypeWrapper::Image, const CameraFileTypeWrapper& fileType = CameraFileTypeWrapper::Normal);
		
		/**
		 * \brief Captures a file from the camera.
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \param[out]	outputFilename	will be used when saving the captured file to disk
		 * \param[in]	autoDeleteImageFromSrc	will remove the file from temporary memory after it's contents have been copied into the cameraFile. The capt0000 is never cleaned up automatically until the camera exists. Most cases will want this set to true, especially long running programs.
		 * \param[in]	captureType	indicates to the camera what file we want (Image, Sound, Movie)
		 * \param[in]	fileType	indicates the type of file to capture from the camera (Raw, Normal, exif, etc...). Most modern cameras can leave this as default.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void capture(CameraWrapper& cameraWrapper, const std::string& outputFilename, bool autoDeleteFileFromSrc = false, const CameraCaptureTypeWrapper& captureType = CameraCaptureTypeWrapper::Image, const CameraFileTypeWrapper& fileType = CameraFileTypeWrapper::Normal);
		
		/**
		 * \brief Captures a file from the camera.
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \param[out]	outputStream	will contain the file data in the ostream memory
		 * \param[in]	autoDeleteImageFromSrc	will remove the file from temporary memory after it's contents have been copied into the cameraFile. The capt0000 is never cleaned up automatically until the camera exists. Most cases will want this set to true, especially long running programs.
		 * \param[in]	captureType	indicates to the camera what file we want (Image, Sound, Movie)
		 * \param[in]	fileType	indicates the type of file to capture from the camera (Raw, Normal, exif, etc...). Most modern cameras can leave this as default.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void capture(CameraWrapper& cameraWrapper, std::ostream& outputStream, bool autoDeleteFileFromSrc = false, const CameraCaptureTypeWrapper& captureType = CameraCaptureTypeWrapper::Image, const CameraFileTypeWrapper& fileType = CameraFileTypeWrapper::Normal);
		
		/**
		 * \brief Recursively scans the camera's filesystem and compiles a list of all folders present.
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \return the list of all folders
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::vector<std::string> getAllFolders(CameraWrapper& cameraWrapper);
		
		/**
		 * \brief Recursively scans the camera's filesystem and compiles a list of all files present.
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \return the list of all files
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::vector<std::string> getAllFiles(CameraWrapper& cameraWrapper);
	}
}

#endif // HELPERCAMERAWRAPPER_HPP
