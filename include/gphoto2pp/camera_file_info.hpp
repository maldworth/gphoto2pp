/** 
 * \file camera_file_info.hpp
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
#ifndef GPHOTO2PP_CAMERA_FILE_INFO_HPP
#define GPHOTO2PP_CAMERA_FILE_INFO_HPP

#include <cstdint>
#include <ctime>
#include <string>

namespace gphoto2pp
{
	enum class CameraFileInfoField : std::uint32_t
	{
		Type = 1u << 0,
		Size = 1u << 2,
		Width = 1u << 3,
		Height = 1u << 4,
		Permissions = 1u << 5,
		Status = 1u << 6,
		ModificationTime = 1u << 7
	};

	enum class CameraFileStatus : int
	{
		NotDownloaded = 0,
		Downloaded = 1
	};

	enum class CameraFilePermission : std::uint32_t
	{
		Read = 1u << 0,
		Delete = 1u << 1
	};

	/**
	 * \brief Common metadata for a file, preview, or audio component.
	 * \note Only values whose fields are marked by has() are valid.
	 * The mask and status preserve values unknown to this wrapper. Size is
	 * in bytes.
	 */
	struct CameraFileInfoPart
	{
		std::uint32_t fields = 0;
		CameraFileStatus status = CameraFileStatus::NotDownloaded;
		std::uint64_t size = 0;
		std::string mimeType;

		/**
		 * \brief Checks whether libgphoto2 marked a field as valid.
		 * \param[in] field field to check
		 * \return true if the field is valid
		 */
		bool has(CameraFileInfoField field) const
		{
			return (fields & static_cast<std::uint32_t>(field)) != 0;
		}
	};

	/**
	 * \brief Metadata for the normal file, including its access and timestamp.
	 * \note The permissions bitmask and modificationTime are valid only when
	 * their corresponding common field flags are set. modificationTime uses
	 * libgphoto2's time_t representation.
	 */
	struct CameraFileInfoFile
	{
		CameraFileInfoPart common;
		std::uint32_t width = 0;
		std::uint32_t height = 0;
		std::uint32_t permissions = 0;
		std::time_t modificationTime = 0;
	};

	/** \brief Metadata for the preview image. */
	struct CameraFileInfoPreview
	{
		CameraFileInfoPart common;
		std::uint32_t width = 0;
		std::uint32_t height = 0;
	};

	/**
	 * \brief Owned snapshot of the file, preview, and audio information.
	 * \note Each component has an independent field-validity mask.
	 */
	struct CameraFileInfo
	{
		CameraFileInfoFile file;
		CameraFileInfoPreview preview;
		CameraFileInfoPart audio;
	};

	/**
	 * \brief Explicit changes to the normal file's writable metadata.
	 * \note The boolean flags select which fields to send, including zero
	 * values. Only CameraFilePermission::Read and ::Delete bits are accepted.
	 * modificationTime uses libgphoto2's time_t representation.
	 */
	struct CameraFileInfoUpdate
	{
		bool updatePermissions = false;
		std::uint32_t permissions = 0;
		bool updateModificationTime = false;
		std::time_t modificationTime = 0;
	};
}

#endif
