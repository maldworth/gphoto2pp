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

#include <gphoto2pp/camera_file_wrapper.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/camera_file_type_wrapper.hpp>
#include <gphoto2pp/exceptions.hpp>

#include <gphoto2pp/log.h>

#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>

namespace gphoto2
{
#include <gphoto2/gphoto2-file.h>
#include <gphoto2/gphoto2-result.h>
}

namespace gphoto2pp
{
	struct CameraFileHandlerState
	{
		CameraFileHandlerCallbacks callbacks;
		gphoto2::CameraFileHandler handler;
	};

	namespace
	{
		int handlerSize(void *privateData, std::uint64_t *size)
		{
			auto *state =
				static_cast<CameraFileHandlerState *>(privateData);
			if (state == nullptr || !state->callbacks.size)
			{
				return GP_ERROR_NOT_SUPPORTED;
			}
			try
			{
				std::uint64_t value = 0;
				const int result = state->callbacks.size(value);
				if (result == GP_OK)
				{
					*size = value;
				}
				return result;
			}
			catch (...)
			{
				return GP_ERROR;
			}
		}

		int handlerRead(void *privateData, unsigned char *data,
			std::uint64_t *length)
		{
			auto *state =
				static_cast<CameraFileHandlerState *>(privateData);
			if (state == nullptr || !state->callbacks.read)
			{
				return GP_ERROR_NOT_SUPPORTED;
			}
			try
			{
				std::uint64_t requested = length == nullptr ? 0 : *length;
				std::vector<char> buffer(
					requested > 0
						? static_cast<std::size_t>(requested) : 0);
				const int result =
					state->callbacks.read(buffer, requested);
				if (result == GP_OK)
				{
					if (length != nullptr)
					{
						if (requested > buffer.size())
						{
							requested = buffer.size();
						}
						*length = requested;
					}
					if (data != nullptr && requested > 0)
					{
						std::memcpy(data, buffer.data(),
							static_cast<std::size_t>(requested));
					}
				}
				return result;
			}
			catch (...)
			{
				return GP_ERROR;
			}
		}

		int handlerWrite(void *privateData, unsigned char *data,
			std::uint64_t *length)
		{
			auto *state =
				static_cast<CameraFileHandlerState *>(privateData);
			if (state == nullptr || !state->callbacks.write)
			{
				return GP_ERROR_NOT_SUPPORTED;
			}
			try
			{
				std::uint64_t requested =
					length == nullptr ? 0 : *length;
				std::vector<char> buffer;
				if (data != nullptr && requested > 0)
				{
					buffer.assign(
						reinterpret_cast<char *>(data),
						reinterpret_cast<char *>(data) +
							static_cast<std::size_t>(requested));
				}
				const int result =
					state->callbacks.write(buffer, requested);
				if (result == GP_OK && length != nullptr)
				{
					*length = requested;
				}
				return result;
			}
			catch (...)
			{
				return GP_ERROR;
			}
		}
	}

	CameraFileWrapper::CameraFileWrapper()
		: m_cameraFile(nullptr)
		, m_handlerState(nullptr)
		, m_callerOwnedData(false)
	{
		FILE_LOG(logINFO) << "CameraFileWrapper Constructor";
		
		gphoto2pp::checkResponse(gphoto2::gp_file_new(&m_cameraFile),"gp_file_new");
	}

	CameraFileWrapper::CameraFileWrapper(int fd)
		: m_cameraFile(nullptr)
		, m_handlerState(nullptr)
		, m_callerOwnedData(true)
	{
		FILE_LOG(logINFO) << "CameraFileWrapper FD Constructor";
		if (fd < 0)
		{
			throw exceptions::ArgumentException(
				"Camera file descriptor must be nonnegative");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_file_new_from_fd(&m_cameraFile, fd),
			"gp_file_new_from_fd");
	}

	CameraFileWrapper::CameraFileWrapper(
		CameraFileHandlerCallbacks const & callbacks)
		: m_cameraFile(nullptr)
		, m_handlerState(std::make_shared<CameraFileHandlerState>())
		, m_callerOwnedData(true)
	{
		FILE_LOG(logINFO) << "CameraFileWrapper Handler Constructor";
		auto *state =
			std::static_pointer_cast<CameraFileHandlerState>(
				m_handlerState).get();
		state->callbacks = callbacks;
		state->handler.size = handlerSize;
		state->handler.read = handlerRead;
		state->handler.write = handlerWrite;
		gphoto2pp::checkResponse(
			gphoto2::gp_file_new_from_handler(
				&m_cameraFile, &state->handler, state),
			"gp_file_new_from_handler");
	}

	CameraFileWrapper::~CameraFileWrapper()
	{
		FILE_LOG(logINFO) << "CameraFileWrapper Destructor";
		
		if(m_cameraFile != nullptr)
		{
			gphoto2pp::checkResponseSilent(gphoto2::gp_file_unref(m_cameraFile),"gp_file_unref");
			m_cameraFile = nullptr;
		}
	}
	
	CameraFileWrapper::CameraFileWrapper(CameraFileWrapper&& other)
		: m_cameraFile(other.m_cameraFile)
		, m_handlerState(std::move(other.m_handlerState))
		, m_callerOwnedData(other.m_callerOwnedData)
	{
		FILE_LOG(logINFO) << "CameraFileWrapper move Constructor";

		other.m_cameraFile = nullptr;
		other.m_handlerState.reset();
		other.m_callerOwnedData = false;
	}
	
	CameraFileWrapper& CameraFileWrapper::operator=(CameraFileWrapper&& other)
	{
		FILE_LOG(logINFO) << "CameraFileWrapper move assignment operator";
		
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraFile != nullptr)
			{
				gphoto2pp::checkResponse(gphoto2::gp_file_unref(m_cameraFile),"gp_file_unref");
			}
			
			// Steal or "move" the other objects resource
			m_cameraFile = other.m_cameraFile;
			m_handlerState = std::move(other.m_handlerState);
			m_callerOwnedData = other.m_callerOwnedData;
			
			// Unreference the other objects resource, so it's destructor doesn't unreference it
			other.m_cameraFile = nullptr;
			other.m_handlerState.reset();
			other.m_callerOwnedData = false;
		}
		return *this;
	}
	
	CameraFileWrapper::CameraFileWrapper(CameraFileWrapper const & other)
		: m_cameraFile(other.m_cameraFile)
		, m_handlerState(other.m_handlerState)
		, m_callerOwnedData(other.m_callerOwnedData)
	{
		FILE_LOG(logINFO) << "CameraFileWrapper copy Constructor";
		
		// Because we now refer to the same file as "other", we need to add to it's reference count
		if(m_cameraFile != nullptr)
		{
			gphoto2::gp_file_ref(m_cameraFile);
		}
	}

	CameraFileWrapper& CameraFileWrapper::operator=(CameraFileWrapper const & other)
	{
		FILE_LOG(logINFO) << "CameraFileWrapper copy assignment operator";
		
		// Check for self assignment
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraFile != nullptr)
			{
				gphoto2pp::checkResponseSilent(gphoto2::gp_file_unref(m_cameraFile),"gp_file_unref"); // FIXED resource leak
				m_cameraFile = nullptr;
			}
			
			// copy the other objects pointer
			m_cameraFile = other.m_cameraFile;
			m_handlerState = other.m_handlerState;
			m_callerOwnedData = other.m_callerOwnedData;
			
			if(m_cameraFile != nullptr)
			{
				gphoto2::gp_file_ref(m_cameraFile);
			}
		}
		return *this;
	}
	
	gphoto2::_CameraFile* CameraFileWrapper::getPtr() const
	{
		return m_cameraFile;
	}

	std::vector<char> CameraFileWrapper::getDataAndSize() const
	{
		FILE_LOG(logDEBUG) << "CameraFileWrapper getDataAndSize";
		
		char const * buffer = nullptr;
		unsigned long int size = 0;
		
		gphoto2pp::checkResponse(gphoto2::gp_file_get_data_and_size(m_cameraFile,&buffer,&size),"gp_file_get_data_and_size");
		
		FILE_LOG(logDEBUG) << "bufferSize: '"<<size<<"'";

		if (size == 0)
		{
			return {};
		}
		if (buffer == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2 returned null file data with a nonzero size");
		}
		std::vector<char> result{buffer, buffer + size};
		if (m_callerOwnedData)
		{
			std::free(const_cast<char *>(buffer));
		}
		return result;
	}


	unsigned long int CameraFileWrapper::getSize() const
	{
		FILE_LOG(logDEBUG) << "CameraFileWrapper getSize";
				
		unsigned long int size;
		
		gphoto2pp::checkResponse(gphoto2::gp_file_get_data_and_size(m_cameraFile, NULL, &size),"gp_file_get_data_and_size");
		
		FILE_LOG(logDEBUG) << "bufferSize: '"<<size<<"'";
		
		return size;
	}
	
	
	void CameraFileWrapper::setDataAndSize(std::vector<char> const & file)
	{
		FILE_LOG(logDEBUG) << "CameraFileWrapper setDataAndSize copy";

		if (file.size() > static_cast<std::size_t>(
			(std::numeric_limits<unsigned long int>::max)()))
		{
			throw exceptions::ArgumentException(
				"Camera file data exceeds libgphoto2's size limit");
		}

		std::unique_ptr<char, decltype(&std::free)> copy(
			file.empty() ? nullptr : static_cast<char *>(
				std::malloc(file.size())), &std::free);
		if (!file.empty() && copy == nullptr)
		{
			throw std::bad_alloc();
		}
		if (!file.empty())
		{
			std::memcpy(copy.get(), file.data(), file.size());
		}

		gphoto2pp::checkResponse(gphoto2::gp_file_set_data_and_size(
			m_cameraFile, copy.get(),
			static_cast<unsigned long int>(file.size())),
			"gp_file_set_data_and_size");
		copy.release();
	}

	void CameraFileWrapper::open(std::string const & filename)
	{
		if (filename.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera file filename must not contain embedded NUL");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_file_open(m_cameraFile, filename.c_str()),
			"gp_file_open");
		m_callerOwnedData = false;
	}

	void CameraFileWrapper::clear()
	{
		gphoto2pp::checkResponse(
			gphoto2::gp_file_clean(m_cameraFile),
			"gp_file_clean");
	}

	void CameraFileWrapper::copyFrom(CameraFileWrapper const & source)
	{
		if (source.getPtr() == nullptr)
		{
			throw exceptions::ArgumentException(
				"Camera file copy source is not initialized");
		}
		if (m_cameraFile == nullptr)
		{
			throw exceptions::ArgumentException(
				"Camera file copy destination is not initialized");
		}
		gphoto2pp::checkResponse(
			gphoto2::gp_file_copy(m_cameraFile, source.getPtr()),
			"gp_file_copy");
	}

	std::string CameraFileWrapper::getMimeType() const
	{
		const char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_file_get_mime_type(m_cameraFile, &temp),"gp_file_get_mime_type");
		
		return std::string(temp);
	}

	void CameraFileWrapper::setMimeType(std::string const & mimeType)
	{
		gphoto2pp::checkResponse(gphoto2::gp_file_set_mime_type(m_cameraFile, mimeType.c_str()),"gp_file_set_mime_type");
	}

	std::string CameraFileWrapper::getFileName() const
	{
		const char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_file_get_name(m_cameraFile, &temp),"gp_file_get_name");
		
		return std::string(temp);
	}

	void CameraFileWrapper::setFileName(std::string const & fileName)
	{
		gphoto2pp::checkResponse(gphoto2::gp_file_set_name(m_cameraFile, fileName.c_str()),"gp_file_set_name");
	}

	void CameraFileWrapper::detectMimeType() const
	{
		gphoto2pp::checkResponse(gphoto2::gp_file_detect_mime_type(m_cameraFile),"gp_file_detect_mime_type");
	}

	void CameraFileWrapper::adjustNameForMimeType()
	{
		gphoto2pp::checkResponse(gphoto2::gp_file_adjust_name_for_mime_type(m_cameraFile),"gp_file_adjust_name_for_mime_type");
	}


	std::string CameraFileWrapper::getNameByType(std::string const & baseName, CameraFileTypeWrapper const & type) const
	{
#ifdef GPHOTO_LESS_25
		throw exceptions::InvalidLinkedVersionException("You are using a version of gphoto2 that doesn't support this command. Please link to gphoto 2.5 or greater");
#else
		char* temp = nullptr;
		
		gphoto2pp::checkResponse(gphoto2::gp_file_get_name_by_type(m_cameraFile, baseName.c_str(), static_cast<gphoto2::CameraFileType>(type), &temp),"gp_file_get_name_by_type");
		
		return std::string(temp);
#endif
	}


	void CameraFileWrapper::save(std::string const & filename) const
	{
		// Not sure if we should expose this. in gphoto2-file.h it mentions this method was never meant for public api usage, but it is handy and lots of people use it... I will add it now for completness
		gphoto2pp::checkResponse(gphoto2::gp_file_save(m_cameraFile, filename.c_str()),"gp_file_save");
		
	}
	
	std::time_t CameraFileWrapper::getMtime() const
	{
		time_t time;
		
		gphoto2pp::checkResponse(gphoto2::gp_file_get_mtime(m_cameraFile, &time),"gp_file_get_mtime");
		
		return time;
	}
	
	void CameraFileWrapper::setMtime(std::time_t mtime)
	{
		gphoto2pp::checkResponse(gphoto2::gp_file_set_mtime(m_cameraFile, mtime),"gp_file_set_mtime");
	}
}

