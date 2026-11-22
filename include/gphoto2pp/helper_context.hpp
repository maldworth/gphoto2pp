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
#include <functional>
#include <string>

namespace gphoto2
{
	#include <gphoto2/gphoto2-context.h>
}


namespace gphoto2pp
{
	enum class ContextFeedback : int
	{
		Ok = 0,
		Cancel = 1
	};

	/**
	 * \brief RAII owner of a libgphoto2 GPContext.
	 * \note Copying is unsupported. Callback registrations remain valid for
	 * the lifetime of the context and use values copied by the callbacks.
	 * If a C++ callback throws, it is logged and translated to the safe
	 * libgphoto2 default: OK/no-cancel or zero progress ID.
	 */
	class ContextWrapper
	{
	public:
		ContextWrapper();
		~ContextWrapper();
		ContextWrapper(ContextWrapper&& other) noexcept;
		ContextWrapper& operator=(ContextWrapper&& other) noexcept;
		ContextWrapper(ContextWrapper const &) = delete;
		ContextWrapper& operator=(ContextWrapper const &) = delete;

		gphoto2::_GPContext* getPtr() const;

		void setIdleCallback(std::function<void()> callback);
		void setErrorCallback(
			std::function<void(std::string const &)> callback);
		void setStatusCallback(
			std::function<void(std::string const &)> callback);
		void setMessageCallback(
			std::function<void(std::string const &)> callback);
		void setQuestionCallback(
			std::function<ContextFeedback(std::string const &)> callback);
		void setCancelCallback(
			std::function<ContextFeedback()> callback);
		void setProgressCallbacks(
			std::function<unsigned int(float, std::string const &)> start,
			std::function<void(unsigned int, float)> update,
			std::function<void(unsigned int)> stop);

		void idle();
		/// \throws GPhoto2pp::exceptions::ArgumentException for embedded NUL
		void reportError(std::string const & text);
		/// \throws GPhoto2pp::exceptions::ArgumentException for embedded NUL
		void reportStatus(std::string const & text);
		/// \throws GPhoto2pp::exceptions::ArgumentException for embedded NUL
		void reportMessage(std::string const & text);
		/// \throws GPhoto2pp::exceptions::ArgumentException for embedded NUL
		ContextFeedback ask(std::string const & text);
		ContextFeedback cancelRequested();
		unsigned int startProgress(
			float target, std::string const & text);
		void updateProgress(unsigned int id, float current);
		void stopProgress(unsigned int id);

	private:
		struct State;
		static void idleCallback(gphoto2::_GPContext *, void *);
		static void errorCallback(
			gphoto2::_GPContext *, const char *, void *);
		static void statusCallback(
			gphoto2::_GPContext *, const char *, void *);
		static void messageCallback(
			gphoto2::_GPContext *, const char *, void *);
		static gphoto2::GPContextFeedback questionCallback(
			gphoto2::_GPContext *, const char *, void *);
		static gphoto2::GPContextFeedback cancelCallback(
			gphoto2::_GPContext *, void *);
		static unsigned int progressStartCallback(
			gphoto2::_GPContext *, float, const char *, void *);
		static void progressUpdateCallback(
			gphoto2::_GPContext *, unsigned int, float, void *);
		static void progressStopCallback(
			gphoto2::_GPContext *, unsigned int, void *);

		gphoto2::_GPContext* requireContext() const;
		std::shared_ptr<State> m_state;
		gphoto2::_GPContext* m_context;
	};

}

#endif // HELPERGPHOTO2_HPP
