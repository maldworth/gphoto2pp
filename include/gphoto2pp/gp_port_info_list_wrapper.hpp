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

#ifndef GPPORTINFOLISTWRAPPER_HPP
#define GPPORTINFOLISTWRAPPER_HPP

#include <string>

namespace gphoto2
{
	struct _GPPortInfoList;
}

namespace gphoto2pp
{
	
	/**
	 * \class GPPortInfoListWrapper
	 * A wrapper around the gphoto2 GPPortInfoList struct.
	 */
	class GPPortInfoListWrapper
	{
	public:
		GPPortInfoListWrapper();
		~GPPortInfoListWrapper();
		
		gphoto2::_GPPortInfoList* getPtr() const;
		
		/**
		 * \brief Finds the list index of the specified port path
		 * \return the port index
		 * \note Direct wrapper for <tt>gp_port_info_list_lookup_path(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupPath(std::string const & path) const;
		
		/**
		 * \brief Finds the list index of the specified model name
		 * \return the port index
		 * \note Direct wrapper for <tt>gp_port_info_list_lookup_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupName(std::string const & name) const;
		
		/**
		 * \brief Gets the number of items in the list
		 * \return the count of items
		 * \note Direct wrapper for <tt>gp_port_info_list_count(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int count() const;
		
	private:
		gphoto2::_GPPortInfoList* m_portInfoList;
	};
}

#endif // GPPORTINFOLISTWRAPPER_HPP
