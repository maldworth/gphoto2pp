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

#ifndef HELPERCONTEXT_HPP
#define HELPERCONTEXT_HPP

#include <memory>

namespace gphoto2
{
	struct _GPContext;
}


namespace gphoto2pp
{
	/**
	 * \brief Creates a shared pointer of GPContext to be used anywhere in the program. Passes in a deleter method which will free the context on destruction.
	 * \return the new context ready for usage
	 * \throw GPhoto2pp::exceptions::NoCameraFoundError if it didn't find any cameras
	 */
	std::shared_ptr<gphoto2::_GPContext> getContext();
	
	namespace helper
	{
		// TODO
	}
}

#endif // HELPERGPHOTO2_HPP
