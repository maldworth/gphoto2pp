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

#include <gphoto2pp/helper_gphoto2.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2-port-info-list.h>
}

namespace gphoto2pp
{

	GPPortInfoListWrapper::GPPortInfoListWrapper()
	{
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_new(&m_portInfoList),"gp_port_info_list_new");
		
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_load(m_portInfoList),"gp_port_info_list_load");
	}

	GPPortInfoListWrapper::~GPPortInfoListWrapper()
	{
		// Destructors should never throw exceptions. Hence the silent response
		gphoto2pp::checkResponseSilent(gphoto2::gp_port_info_list_free(m_portInfoList),"gp_port_info_list_free");
	}
	
	gphoto2::_GPPortInfoList* GPPortInfoListWrapper::getPtr() const
	{
		return m_portInfoList;
	}

	int GPPortInfoListWrapper::lookupPath(std::string const & path) const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_lookup_path(m_portInfoList, path.c_str()),"gp_port_info_list_lookup_path");
	}
	
	int GPPortInfoListWrapper::lookupName(std::string const & name) const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_lookup_name(m_portInfoList, name.c_str()),"gp_port_info_list_lookup_name");
	}
	
	int GPPortInfoListWrapper::count() const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_count(m_portInfoList),"gp_port_info_list_count");
	}

}

