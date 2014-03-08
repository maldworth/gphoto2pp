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

#ifndef CAMERAFILEWRAPPER_HPP
#define CAMERAFILEWRAPPER_HPP

#include <string>
#include <ctime>
#include <vector>

namespace gphoto2
{
	struct _CameraFile;
}

namespace gphoto2pp
{
	//Forward Declarations
	enum class CameraFileTypeWrapper : int;
	
	/**
	 * \class CameraFileWrapper
	 * A wrapper around the gphoto2 CameraFile struct.
	 */
	class CameraFileWrapper
	{
	public:
		CameraFileWrapper();
		~CameraFileWrapper();
		
		// Move constructor and move assignment
		CameraFileWrapper(CameraFileWrapper&& other);
		CameraFileWrapper& operator=(CameraFileWrapper&& other);
		
		// Copy constructor and copy assignment
		/**
		 * \note this copy constructor does not actually copy the image, but rather makes a copy of this wrapper around the gphoto2 struct CameraFile. 
		 */
		CameraFileWrapper(CameraFileWrapper const & other);
		
		/**
		 * \note this copy assignments does not actually copy the image, but rather makes a copy of this wrapper around the gphoto2 struct CameraFile. 
		 */
		CameraFileWrapper& operator=( CameraFileWrapper const & other );
		
		gphoto2::_CameraFile* getPtr() const;
		
		/**
		 * \brief Gets the file's binary data in a std::string.
		 * This structure is acceptable for transportation of the camera file, as it has a size() method and c_str() to retrieve the relevant binary picture data.
		 * \return the picture
		 * \note Direct wrapper for <tt>gp_file_get_data_and_size(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::vector<char> getDataAndSize() const;
		
		/**
		 * \brief Sets the camera file's binary data
		 * \param[in]	file	which will be written into the gphoto2 CameraFile struct
		 * \note Direct wrapper for <tt>gp_file_set_data_and_size(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setDataAndSize(std::vector<char> const & file);
		
		/**
		 * \brief Gets the file's MIME type
		 * \return the mime type
		 * \note Direct wrapper for <tt>gp_file_get_mime_type(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getMimeType() const;
		
		/**
		 * \brief Sets the file's MIME type
		 * \param[in]	mimeType	to set for this file
		 * \note Direct wrapper for <tt>gp_file_set_mime_type(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setMimeType(std::string const & mimeType);
		
		/**
		 * \brief Gets the file's name
		 * \return the file name
		 * \note Direct wrapper for <tt>gp_file_get_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getFileName() const;
		
		/**
		 * \brief Sets the file's name
		 * \param[in]	fileName	to set for this file
		 * \note Direct wrapper for <tt>gp_file_set_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setFileName(std::string const & fileName);
		
		/**
		 * \brief Detects the MIME type and updates the internal structures MIME type.
		 * If the MIME type is still unknown/unknown after this, then gphoto2 does not recognize the file type.
		 * \note Direct wrapper for <tt>gp_file_detect_mime_type(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void detectMimeType() const;
		
		/**
		 * \brief Adjusts the file name to match the detected MIME type.
		 * \par Recognized MIME Types:
		 * - RAW -> raw
		 * - JPEG -> jpg
		 * - PNG -> png
		 * - PPM -> ppm
		 * - PGM -> pgm
		 * - PNM -> pnm
		 * - TIFF -> tif
		 * - WAV -> wav
		 * - BMP -> bmp
		 * - AVI -> avi
		 * - CRW -> crw
		 * - CR2 -> cr2
		 * \note Direct wrapper for <tt>gp_file_adjust_name_for_mime_type(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void adjustNameForMimeType();
		
		/**
		 * \brief Gets the timestamp of the file.
		 * \note Direct wrapper for <tt>gp_file_get_mtime(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::time_t getMtime() const;
		
		/**
		 * \brief Sets the timestamp of the file.
		 * \param[in]	mtime	to set for this file
		 * \note Direct wrapper for <tt>gp_file_set_mtime(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setMtime(std::time_t mtime);
		
		/**
		 * \brief Generates a file name using the file type, basename, and mime type.
		 * \param[in]	baseName	for this file, which will appear after the file type and before the mime type
		 * \param[in]	type		of file that was captured (Image, Movie, Sound)
		 * \note Direct wrapper for <tt>gp_file_get_name_by_type(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getNameByType(std::string const & baseName, CameraFileTypeWrapper const & type) const; // This command is only supported by GPhoto >= 2.5
		
		/**
		 * \brief Saves the current file to the current executing directory with the provided filename.
		 * \param[in]	filename	to save as, ignoring any filename that may be set already
		 * \note Direct wrapper for <tt>gp_file_save(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void save(std::string const & filename) const;

	private:
		gphoto2::_CameraFile* m_cameraFile;

	};

}

#endif // CAMERAFILEWRAPPER_HPP
