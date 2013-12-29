#include "helper_context.hpp"

#include "log.h"

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
