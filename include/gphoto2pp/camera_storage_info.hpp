/** 
 * \file camera_storage_info.hpp
 *
 * \author Copyright (c) 2026 Vitor Fonseca <https://github.com/VSF1>
 *
 * \note This file is part of gphoto2pp
 *
 * \note gphoto2pp is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * \note gphoto2pp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * \note You should have received a copy of the GNU Lesser General Public
 * License along with gphoto2pp. If not, see http://www.gnu.org/licenses
 */
#ifndef GPHOTO2PP_CAMERA_STORAGE_INFO_HPP
#define GPHOTO2PP_CAMERA_STORAGE_INFO_HPP

#include <cstdint>
#include <string>

namespace gphoto2pp
{
	enum class CameraStorageInfoField : std::uint32_t
	{
		Base = 1u << 0,
		Label = 1u << 1,
		Description = 1u << 2,
		Access = 1u << 3,
		StorageType = 1u << 4,
		FilesystemType = 1u << 5,
		MaxCapacity = 1u << 6,
		FreeSpaceKBytes = 1u << 7,
		FreeSpaceImages = 1u << 8
	};

	enum class CameraStorageType : int
	{
		Unknown = 0, FixedRom = 1, RemovableRom = 2,
		FixedRam = 3, RemovableRam = 4
	};

	enum class CameraStorageFilesystemType : int
	{
		Undefined = 0, GenericFlat = 1, GenericHierarchical = 2, Dcf = 3
	};

	enum class CameraStorageAccessType : int
	{
		ReadWrite = 0, ReadOnly = 1, ReadOnlyWithDelete = 2
	};

	/**
	 * \brief Owned snapshot of one camera storage entry.
	 * \note Only fields indicated by has() are valid. Fields retains unknown
	 * flag bits, and enum values not known to this wrapper are preserved.
	 * Capacities are in kbytes; freeImages is the camera's estimate.
	 */
	struct CameraStorageInfo
	{
		std::uint32_t fields = 0;
		std::string baseDirectory;
		std::string label;
		std::string description;
		CameraStorageType storageType = CameraStorageType::Unknown;
		CameraStorageFilesystemType filesystemType = CameraStorageFilesystemType::Undefined;
		CameraStorageAccessType access = CameraStorageAccessType::ReadWrite;
		std::uint64_t capacityKBytes = 0;
		std::uint64_t freeKBytes = 0;
		std::uint64_t freeImages = 0;

		/**
		 * \brief Checks whether libgphoto2 marked a field as valid.
		 * \param[in] field field to check
		 * \return true if the field is valid
		 */
		bool has(CameraStorageInfoField field) const
		{
			return (fields & static_cast<std::uint32_t>(field)) != 0;
		}
	};
}

#endif
