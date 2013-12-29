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
