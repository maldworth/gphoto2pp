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

#ifndef CAMERAABILITIESLISTWRAPPER_H
#define CAMERAABILITIESLISTWRAPPER_H

#include <cstdint>
#include <string>
#include <vector>

namespace gphoto2
{
	// Forward Declaration
	struct _CameraAbilitiesList;
}

namespace gphoto2pp
{
	class GPPortInfoListWrapper;
	class CameraListWrapper;

	enum class CameraDriverStatus : int
	{
		Production = 0,
		Testing = 1,
		Experimental = 2,
		Deprecated = 3
	};

	enum class GphotoDeviceType : int
	{
		StillCamera = 0,
		AudioPlayer = 1 << 0
	};

	enum class CameraOperation : std::uint32_t
	{
		None = 0,
		CaptureImage = 1 << 0,
		CaptureVideo = 1 << 1,
		CaptureAudio = 1 << 2,
		CapturePreview = 1 << 3,
		Config = 1 << 4,
		TriggerCapture = 1 << 5
	};

	enum class CameraFileOperation : std::uint32_t
	{
		None = 0,
		Delete = 1 << 1,
		Preview = 1 << 3,
		Raw = 1 << 4,
		Audio = 1 << 5,
		Exif = 1 << 6
	};

	enum class CameraFolderOperation : std::uint32_t
	{
		None = 0,
		DeleteAll = 1 << 0,
		PutFile = 1 << 1,
		MakeDir = 1 << 2,
		RemoveDir = 1 << 3
	};

	struct CameraAbilitiesSnapshot
	{
		std::string model;
		CameraDriverStatus status = CameraDriverStatus::Production;
		std::uint32_t port = 0;
		std::vector<int> speeds;
		std::uint32_t operations = 0;
		std::uint32_t fileOperations = 0;
		std::uint32_t folderOperations = 0;
		int usbVendor = 0;
		int usbProduct = 0;
		int usbClass = 0;
		int usbSubclass = 0;
		int usbProtocol = 0;
		std::string library;
		std::string id;
		GphotoDeviceType deviceType = GphotoDeviceType::StillCamera;
	};
	
	/**
	 * \class CameraAbilitiesListWrapper
	 * A wrapper around the gphoto2 CameraAbilitiesList struct.
	 */
	class CameraAbilitiesListWrapper
	{
	public:
		CameraAbilitiesListWrapper();
		~CameraAbilitiesListWrapper();
		
		// Move constructor and move assignment are allowed
		CameraAbilitiesListWrapper(CameraAbilitiesListWrapper&& other);
		CameraAbilitiesListWrapper& operator=(CameraAbilitiesListWrapper&& other);
		
		// We cannot support copy constructor or assignment at this time
		CameraAbilitiesListWrapper(CameraAbilitiesListWrapper const & other) = delete;
		CameraAbilitiesListWrapper& operator=(CameraAbilitiesListWrapper const & other) = delete;
		
		gphoto2::_CameraAbilitiesList* getPtr() const;

		/**
		 * \brief Returns an owned snapshot of one abilities entry.
		 * \param[in] index zero-based abilities index
		 * \return Copied model, status, ports, operations, USB and device data.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception for an invalid index
		 * or libgphoto2 failure
		 * \throw std::runtime_error if fixed strings are unterminated
		 */
		CameraAbilitiesSnapshot getAbilities(int index) const;

		/**
		 * \brief Loads camera drivers from a directory.
		 * \param[in] directory directory containing libgphoto2 camera modules
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL bytes
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 */
		void loadDirectory(std::string const & directory);

		/**
		 * \brief Appends a camera abilities entry.
		 * \param[in] abilities owned snapshot to append
		 * \throw GPhoto2pp::exceptions::ArgumentException for malformed strings
		 * or an oversize serial port speed list
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 */
		void append(CameraAbilitiesSnapshot const & abilities);
		
		/**
		 * \brief Tries to detect any cameras connected to the computer using the list of ports provided
		 * \param[in]	portInfoList	of ports to scan for camera models
		 * \note Direct wrapper for <tt>gp_abilities_list_detect(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraListWrapper listDetect(GPPortInfoListWrapper const & portInfoList);
		
		/**
		 * \brief Resets the abilities list
		 * \note Direct wrapper for <tt>gp_abilities_list_reset(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void reset();
		
		/**
		 * \brief Counts the entries in the abilities list
		 * \return he number of items in the list
		 * \note Direct wrapper for <tt>gp_abilities_list_count(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int count() const;
		
		/**
		 * \brief Scans the list for the given model
		 * \param[in]	model	of camera to search for
		 * \return the index of the item (if found)
		 * \note Direct wrapper for <tt>gp_abilities_list_lookup_model(...)</tt>
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL bytes
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupModel(std::string const & model) const;
		
	private:
		gphoto2::_CameraAbilitiesList* requireList() const;
		gphoto2::_CameraAbilitiesList* m_cameraAbilitiesList;
	};
}

#endif // CAMERAABILITIESLISTWRAPPER_H
