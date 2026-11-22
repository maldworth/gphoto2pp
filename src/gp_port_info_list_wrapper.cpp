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

#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/helper_gphoto2.hpp>

#include <memory>
#include <stdexcept>

namespace gphoto2
{
#include <gphoto2/gphoto2-port-info-list.h>
}

namespace gphoto2pp
{

	GPPortInfoListWrapper::GPPortInfoListWrapper()
		: m_portInfoList(nullptr)
	{
		gphoto2::_GPPortInfoList *rawList = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_new(&rawList),
			"gp_port_info_list_new");
		if (rawList == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2 returned a null port information list");
		}
		auto cleanup = [](gphoto2::_GPPortInfoList *list)
		{
			gphoto2pp::checkResponseSilent(
				gphoto2::gp_port_info_list_free(list), "gp_port_info_list_free");
		};
		std::unique_ptr<gphoto2::_GPPortInfoList, decltype(cleanup)> list(
			rawList, cleanup);
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_load(list.get()),
			"gp_port_info_list_load");
		m_portInfoList = list.release();
	}

	GPPortInfoListWrapper::~GPPortInfoListWrapper()
	{
		if (m_portInfoList != nullptr)
		{
			gphoto2pp::checkResponseSilent(
				gphoto2::gp_port_info_list_free(m_portInfoList),
				"gp_port_info_list_free");
		}
	}

	GPPortInfoListWrapper::GPPortInfoListWrapper(
		GPPortInfoListWrapper&& other) noexcept
		: m_portInfoList(other.m_portInfoList)
	{
		other.m_portInfoList = nullptr;
	}

	GPPortInfoListWrapper& GPPortInfoListWrapper::operator=(
		GPPortInfoListWrapper&& other)
	{
		if (this != &other)
		{
			if (m_portInfoList != nullptr)
			{
				const int result =
					gphoto2::gp_port_info_list_free(m_portInfoList);
				if (result < 0)
				{
					gphoto2pp::checkResponse(result, "gp_port_info_list_free");
				}
			}
			m_portInfoList = other.m_portInfoList;
			other.m_portInfoList = nullptr;
		}
		return *this;
	}
	
	gphoto2::_GPPortInfoList* GPPortInfoListWrapper::getPtr() const
	{
		return m_portInfoList;
	}

	gphoto2::_GPPortInfoList* GPPortInfoListWrapper::requireList() const
	{
		if (m_portInfoList == nullptr)
		{
			throw std::logic_error("Port information list has been moved from");
		}
		return m_portInfoList;
	}

	int GPPortInfoListWrapper::lookupPath(std::string const & path) const
	{
		auto *list = this->requireList();
		if (path.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Port path must not contain embedded NUL bytes");
		}
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_lookup_path(list, path.c_str()),"gp_port_info_list_lookup_path");
	}
	
	int GPPortInfoListWrapper::lookupName(std::string const & name) const
	{
		auto *list = this->requireList();
		if (name.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Port name must not contain embedded NUL bytes");
		}
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_lookup_name(list, name.c_str()),"gp_port_info_list_lookup_name");
	}
	
	int GPPortInfoListWrapper::count() const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_count(this->requireList()),"gp_port_info_list_count");
	}

	PortInfoSnapshot GPPortInfoListWrapper::getInfo(int index) const
	{
		gphoto2::GPPortInfo info = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_get_info(
			this->requireList(), index, &info), "gp_port_info_list_get_info");
		if (info == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2 returned a null port information entry");
		}

		char *name = nullptr;
		char *path = nullptr;
		gphoto2::GPPortType type = gphoto2::GP_PORT_NONE;
		gphoto2pp::checkResponse(gphoto2::gp_port_info_get_name(
			info, &name), "gp_port_info_get_name");
		gphoto2pp::checkResponse(gphoto2::gp_port_info_get_path(
			info, &path), "gp_port_info_get_path");
		gphoto2pp::checkResponse(gphoto2::gp_port_info_get_type(
			info, &type), "gp_port_info_get_type");
		if (name == nullptr || path == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2 returned incomplete port information");
		}

		PortInfoSnapshot snapshot;
		snapshot.name = name;
		snapshot.path = path;
		snapshot.type = static_cast<GPPortTypeWrapper>(type);
		return snapshot;
	}

}

