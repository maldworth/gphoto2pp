/**
 * \file camera_abilities_list_wrapper.cpp
 *
 * \author Copyright (c) 2013 maldworth <https://github.com/maldworth>
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

#include <gphoto2pp/camera_abilities_list_wrapper.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/helper_context.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

#include <gphoto2pp/log.h>
#include <gphoto2pp/exceptions.hpp>

#include <cstring>
#include <memory>
#include <stdexcept>

namespace gphoto2
{
#include <gphoto2/gphoto2-abilities-list.h>
}

namespace gphoto2pp
{

	CameraAbilitiesListWrapper::CameraAbilitiesListWrapper()
		: m_cameraAbilitiesList{nullptr}
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper Constructor";

		gphoto2::_CameraAbilitiesList *rawList = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_new(&rawList),
			"gp_abilities_list_new");
		if (rawList == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2 returned a null camera abilities list");
		}
		std::unique_ptr<gphoto2::_CameraAbilitiesList,
			void (*)(gphoto2::_CameraAbilitiesList *)> list(
				rawList,
				[](gphoto2::_CameraAbilitiesList *cameraList)
				{
					gphoto2pp::checkResponseSilent(
						gphoto2::gp_abilities_list_free(cameraList),
						"gp_abilities_list_free");
				});
		ContextWrapper context;
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_load(
			list.get(), context.getPtr()), "gp_abilities_list_load");
		m_cameraAbilitiesList = list.release();
	}

	CameraAbilitiesListWrapper::~CameraAbilitiesListWrapper()
	{
		FILE_LOG(logINFO) << "~CameraAbilitiesListWrapper Destructor";

		if(m_cameraAbilitiesList != nullptr)
		{
			gphoto2pp::checkResponseSilent(
                gphoto2::gp_abilities_list_free(m_cameraAbilitiesList),
                "gp_abilities_list_free");
			m_cameraAbilitiesList = nullptr;
		}
	}
	
	CameraAbilitiesListWrapper::CameraAbilitiesListWrapper(
        CameraAbilitiesListWrapper&& other)
		: m_cameraAbilitiesList{other.m_cameraAbilitiesList}
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move Constructor";
		
		other.m_cameraAbilitiesList = nullptr;
	}
	
	CameraAbilitiesListWrapper& CameraAbilitiesListWrapper::operator=(
        CameraAbilitiesListWrapper&& other)
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move assignment operator";

		if(this != &other)
		{
			if(m_cameraAbilitiesList != nullptr)
			{
				gphoto2pp::checkResponse(
					gphoto2::gp_abilities_list_free(m_cameraAbilitiesList),
					"gp_abilities_list_free");
			}
			m_cameraAbilitiesList = other.m_cameraAbilitiesList;
			other.m_cameraAbilitiesList = nullptr;
		}
		return *this;
	}

	gphoto2::_CameraAbilitiesList* CameraAbilitiesListWrapper::getPtr() const
	{
		return m_cameraAbilitiesList;
	}

	gphoto2::_CameraAbilitiesList*
	CameraAbilitiesListWrapper::requireList() const
	{
		if (m_cameraAbilitiesList == nullptr)
		{
			throw std::logic_error("Camera abilities list has been moved from");
		}
		return m_cameraAbilitiesList;
	}

	namespace
	{
		std::string fixedString(char const *value, std::size_t size,
			char const *fieldName)
		{
			if (value == nullptr)
			{
				throw std::runtime_error(
					std::string("libgphoto2 returned null ") + fieldName);
			}
			const auto *end = static_cast<char const *>(
				std::memchr(value, '\0', size));
			if (end == nullptr)
			{
				throw std::runtime_error(
					std::string("libgphoto2 returned unterminated ") + fieldName);
			}
			return std::string(value, static_cast<std::size_t>(end - value));
		}
	}

	CameraAbilitiesSnapshot CameraAbilitiesListWrapper::getAbilities(
		int index) const
	{
		gphoto2::CameraAbilities source{};
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_get_abilities(
			this->requireList(), index, &source),
			"gp_abilities_list_get_abilities");

		CameraAbilitiesSnapshot snapshot;
		snapshot.model = fixedString(source.model, sizeof(source.model),
			"camera abilities model");
		snapshot.status = static_cast<CameraDriverStatus>(source.status);
		snapshot.port = static_cast<std::uint32_t>(source.port);
		for (int speed : source.speed)
		{
			if (speed == 0)
			{
				break;
			}
			snapshot.speeds.push_back(speed);
		}
		snapshot.operations = static_cast<std::uint32_t>(source.operations);
		snapshot.fileOperations =
			static_cast<std::uint32_t>(source.file_operations);
		snapshot.folderOperations =
			static_cast<std::uint32_t>(source.folder_operations);
		snapshot.usbVendor = source.usb_vendor;
		snapshot.usbProduct = source.usb_product;
		snapshot.usbClass = source.usb_class;
		snapshot.usbSubclass = source.usb_subclass;
		snapshot.usbProtocol = source.usb_protocol;
		snapshot.library = fixedString(source.library, sizeof(source.library),
			"camera abilities library");
		snapshot.id = fixedString(source.id, sizeof(source.id),
			"camera abilities id");
		snapshot.deviceType =
			static_cast<GphotoDeviceType>(source.device_type);
		return snapshot;
	}

	void CameraAbilitiesListWrapper::loadDirectory(
		std::string const & directory)
	{
		auto *list = this->requireList();
		if (directory.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera abilities directory must not contain embedded NUL");
		}
		ContextWrapper context;
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_load_dir(
			list, directory.c_str(), context.getPtr()),
			"gp_abilities_list_load_dir");
	}

	void CameraAbilitiesListWrapper::append(
		CameraAbilitiesSnapshot const & abilities)
	{
		auto *list = this->requireList();
		if (abilities.model.find('\0') != std::string::npos ||
			abilities.library.find('\0') != std::string::npos ||
			abilities.id.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera abilities strings must not contain embedded NUL");
		}
		if (abilities.model.size() >= 128 ||
			abilities.library.size() >= 1024 ||
			abilities.id.size() >= 1024 ||
			abilities.speeds.size() >= 64)
		{
			throw exceptions::ArgumentException(
				"Camera abilities snapshot exceeds libgphoto2 fixed limits");
		}

		gphoto2::CameraAbilities source{};
		std::memcpy(source.model, abilities.model.c_str(),
			abilities.model.size());
		source.status = static_cast<gphoto2::CameraDriverStatus>(
			abilities.status);
		source.port = static_cast<gphoto2::GPPortType>(abilities.port);
		for (std::size_t i = 0; i < abilities.speeds.size(); ++i)
		{
			source.speed[i] = abilities.speeds[i];
		}
		source.operations =
			static_cast<gphoto2::CameraOperation>(abilities.operations);
		source.file_operations =
			static_cast<gphoto2::CameraFileOperation>(
				abilities.fileOperations);
		source.folder_operations =
			static_cast<gphoto2::CameraFolderOperation>(
				abilities.folderOperations);
		source.usb_vendor = abilities.usbVendor;
		source.usb_product = abilities.usbProduct;
		source.usb_class = abilities.usbClass;
		source.usb_subclass = abilities.usbSubclass;
		source.usb_protocol = abilities.usbProtocol;
		std::memcpy(source.library, abilities.library.c_str(),
			abilities.library.size());
		std::memcpy(source.id, abilities.id.c_str(), abilities.id.size());
		source.device_type =
			static_cast<gphoto2::GphotoDeviceType>(abilities.deviceType);
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_append(
			list, source),
			"gp_abilities_list_append");
	}

	CameraListWrapper CameraAbilitiesListWrapper::listDetect(
        GPPortInfoListWrapper const & portInfoList)
	{
		auto *list = this->requireList();
		ContextWrapper context;
		CameraListWrapper cameraList;

		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_detect(
			list, portInfoList.getPtr(), cameraList.getPtr(),
			context.getPtr()), "gp_abilities_list_detect");
		return cameraList; // Compiler should do RVO here
	}

	void CameraAbilitiesListWrapper::reset()
	{
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_reset(
			this->requireList()), "gp_abilities_list_reset");
	}

	int CameraAbilitiesListWrapper::count() const
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper count";

		return gphoto2pp::checkResponse(gphoto2::gp_abilities_list_count(
			this->requireList()), "gp_abilities_list_count");
	}

	// Meant to return the index of the specified model, or gphoto error code
	int CameraAbilitiesListWrapper::lookupModel(std::string const & model) const
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper lookupModel - model";

		if (model.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera model must not contain embedded NUL");
		}
		return gphoto2pp::checkResponse(
			gphoto2::gp_abilities_list_lookup_model(
				this->requireList(), model.c_str()),
			"gp_abilities_list_lookup_model");
	}

}

