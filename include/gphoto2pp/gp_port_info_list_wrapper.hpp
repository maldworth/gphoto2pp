/**
 * \file gp_port_info_list_wrapper.hpp
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

#ifndef GPPORTINFOLISTWRAPPER_HPP
#define GPPORTINFOLISTWRAPPER_HPP

#include <string>

namespace gphoto2
{
	struct _GPPortInfoList;
}

namespace gphoto2pp
{
	enum class GPPortTypeWrapper : int
	{
		None = 0,
		Serial = 1 << 0,
		USB = 1 << 2,
		Disk = 1 << 3,
		PTPIP = 1 << 4,
		USBDiskDirect = 1 << 5,
		USBScsi = 1 << 6,
		IP = 1 << 7
	};

	/**
	 * \brief Owned snapshot of one port entry.
	 * \note The type preserves libgphoto2 values unknown to this wrapper.
	 */
	struct PortInfoSnapshot
	{
		std::string name;
		std::string path;
		GPPortTypeWrapper type = GPPortTypeWrapper::None;
	};

	/**
	 * \class GPPortInfoListWrapper
	 * A move-only owner of a gphoto2 GPPortInfoList.
	 * A moved-from wrapper may be destroyed or assigned to; getPtr() returns
	 * null and the other operations throw std::logic_error.
	 */
	class GPPortInfoListWrapper
	{
	public:
		/**
		 * \brief Creates and loads a port information list.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw std::runtime_error if libgphoto2 returns a null list
		 */
		GPPortInfoListWrapper();
		~GPPortInfoListWrapper();

		/// Transfers ownership, leaving other empty.
		GPPortInfoListWrapper(GPPortInfoListWrapper&& other) noexcept;
		/**
		 * \brief Releases the current list and transfers ownership from other.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception if releasing the
		 * current list fails; other is not consumed in that case
		 */
		GPPortInfoListWrapper& operator=(GPPortInfoListWrapper&& other);
		GPPortInfoListWrapper(GPPortInfoListWrapper const &) = delete;
		GPPortInfoListWrapper& operator=(GPPortInfoListWrapper const &) = delete;

		/**
		 * \brief Gets the borrowed underlying list, or null after a move.
		 * \note The caller must not free the pointer. It becomes invalid when
		 * its owning wrapper releases the list.
		 */
		gphoto2::_GPPortInfoList* getPtr() const;
		
		/**
		 * \brief Finds the list index of the specified port path
		 * \param[in] path port path without embedded NUL bytes
		 * \return the port index
		 * \note Direct wrapper for <tt>gp_port_info_list_lookup_path(...)</tt>
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * bytes
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupPath(std::string const & path) const;
		
		/**
		 * \brief Finds the list index of the specified model name
		 * \param[in] name port name without embedded NUL bytes
		 * \return the port index
		 * \note Direct wrapper for <tt>gp_port_info_list_lookup_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * bytes
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

		/**
		 * \brief Gets an owned snapshot of one port entry.
		 * \param[in] index zero-based index in this list
		 * \return Copied name, path, and port type.
		 * \note The snapshot does not depend on this list's lifetime.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception for an invalid index
		 * or libgphoto2 failure
		 * \throw std::runtime_error if libgphoto2 returns incomplete metadata
		 */
		PortInfoSnapshot getInfo(int index) const;
		
	private:
		gphoto2::_GPPortInfoList* requireList() const;
		gphoto2::_GPPortInfoList* m_portInfoList;
	};
}

#endif // GPPORTINFOLISTWRAPPER_HPP
