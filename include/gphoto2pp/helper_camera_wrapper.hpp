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

#ifndef HELPERCAMERAWRAPPER_HPP
#define HELPERCAMERAWRAPPER_HPP

#include <gphoto2pp/camera_file_type_wrapper.hpp>
#include <gphoto2pp/camera_capture_type_wrapper.hpp>

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
		void capturePreview(CameraWrapper& cameraWrapper, std::string const & outputFilename);
		
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
		void capture(CameraWrapper& cameraWrapper, CameraFileWrapper& cameraFile, bool autoDeleteFileFromSrc = false, CameraCaptureTypeWrapper const & captureType = CameraCaptureTypeWrapper::Image, CameraFileTypeWrapper const & fileType = CameraFileTypeWrapper::Normal);
		
		/**
		 * \brief Captures a file from the camera.
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \param[out]	outputFilename	will be used when saving the captured file to disk
		 * \param[in]	autoDeleteImageFromSrc	will remove the file from temporary memory after it's contents have been copied into the cameraFile. The capt0000 is never cleaned up automatically until the camera exists. Most cases will want this set to true, especially long running programs.
		 * \param[in]	captureType	indicates to the camera what file we want (Image, Sound, Movie)
		 * \param[in]	fileType	indicates the type of file to capture from the camera (Raw, Normal, exif, etc...). Most modern cameras can leave this as default.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void capture(CameraWrapper& cameraWrapper, std::string const & outputFilename, bool autoDeleteFileFromSrc = false, CameraCaptureTypeWrapper const & captureType = CameraCaptureTypeWrapper::Image, CameraFileTypeWrapper const & fileType = CameraFileTypeWrapper::Normal);
		
		/**
		 * \brief Captures a file from the camera.
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \param[out]	outputStream	will contain the file data in the ostream memory
		 * \param[in]	autoDeleteImageFromSrc	will remove the file from temporary memory after it's contents have been copied into the cameraFile. The capt0000 is never cleaned up automatically until the camera exists. Most cases will want this set to true, especially long running programs.
		 * \param[in]	captureType	indicates to the camera what file we want (Image, Sound, Movie)
		 * \param[in]	fileType	indicates the type of file to capture from the camera (Raw, Normal, exif, etc...). Most modern cameras can leave this as default.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void capture(CameraWrapper& cameraWrapper, std::ostream& outputStream, bool autoDeleteFileFromSrc = false, CameraCaptureTypeWrapper const & captureType = CameraCaptureTypeWrapper::Image, CameraFileTypeWrapper const & fileType = CameraFileTypeWrapper::Normal);
		
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
		
		/**
		 * \brief Canon cameras need this set to *true* to allow pictures to save to the sd card
		 * \param[in]	cameraWrapper	instance which will be used to issue the capture command
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setCanonCapture(CameraWrapper& cameraWrapper, bool capture);
	}
}

#endif // HELPERCAMERAWRAPPER_HPP
