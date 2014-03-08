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

#include <string>

namespace gphoto2
{
	// Forward Declaration
	struct _CameraAbilitiesList;
}

namespace gphoto2pp
{
	class GPPortInfoListWrapper;
	class CameraListWrapper;
	
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
		 * \note Direct wrapper for <tt>gp_abilities_list_count(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupModel(std::string const & model) const;
		
	private:
		gphoto2::_CameraAbilitiesList* m_cameraAbilitiesList;
	};
}

#endif // CAMERAABILITIESLISTWRAPPER_H
