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

#include <gphoto2pp/helper_context.hpp>

namespace gphoto2
{
#include <gphoto2/gphoto2-context.h>
}

namespace gphoto2pp
{
	std::shared_ptr<gphoto2::_GPContext> getContext()
	{
		auto deleter = [](gphoto2::_GPContext* p){gphoto2::gp_context_unref(p);};
		
		gphoto2::_GPContext* gpContext = gphoto2::gp_context_new();
		
		return std::shared_ptr<gphoto2::_GPContext>(gpContext, deleter);
	}
	
	// These are additional helper methods which are for more advanced users of libgphoto2
	namespace context
	{
		
	}
}
