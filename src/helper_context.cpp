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

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/log.h>

#include <mutex>
#include <stdexcept>
#include <utility>

namespace gphoto2
{
#include <gphoto2/gphoto2-context.h>
}

namespace gphoto2pp
{
	struct ContextWrapper::State
	{
		std::mutex mutex;
		std::function<void()> idle;
		std::function<void(std::string const &)> error;
		std::function<void(std::string const &)> status;
		std::function<void(std::string const &)> message;
		std::function<ContextFeedback(std::string const &)> question;
		std::function<ContextFeedback()> cancel;
		std::function<unsigned int(float, std::string const &)> progressStart;
		std::function<void(unsigned int, float)> progressUpdate;
		std::function<void(unsigned int)> progressStop;
	};

	namespace
	{
		template <typename Callback>
		Callback copyCallback(
			std::mutex &mutex, Callback const &callback)
		{
			std::lock_guard<std::mutex> lock(mutex);
			return callback;
		}

	}

	void ContextWrapper::idleCallback(
		gphoto2::_GPContext *, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback = copyCallback(state->mutex, state->idle);
		if (callback)
		{
			try
			{
				callback();
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "libgphoto2 idle callback threw a C++ exception";
			}
		}
	}

	void ContextWrapper::statusCallback(
		gphoto2::_GPContext *, const char *text, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback = copyCallback(state->mutex, state->status);
		if (callback)
		{
			try
			{
				callback(text == nullptr ? std::string{} : std::string(text));
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "libgphoto2 status callback threw a C++ exception";
			}
		}
	}

	void ContextWrapper::messageCallback(
		gphoto2::_GPContext *, const char *text, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback = copyCallback(state->mutex, state->message);
		if (callback)
		{
			try
			{
				callback(text == nullptr ? std::string{} : std::string(text));
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "libgphoto2 message callback threw a C++ exception";
			}
		}
	}

	void ContextWrapper::errorCallback(
		gphoto2::_GPContext *, const char *text, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback = copyCallback(state->mutex, state->error);
		if (callback)
		{
			try
			{
				callback(text == nullptr ? std::string{} : std::string(text));
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "libgphoto2 error callback threw a C++ exception";
			}
		}
	}

	gphoto2::GPContextFeedback ContextWrapper::questionCallback(
		gphoto2::_GPContext *, const char *text, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback = copyCallback(state->mutex, state->question);
		if (!callback)
		{
			return gphoto2::GP_CONTEXT_FEEDBACK_OK;
		}
		try
		{
			return callback(
				text == nullptr ? std::string{} : std::string(text))
				== ContextFeedback::Cancel
				? gphoto2::GP_CONTEXT_FEEDBACK_CANCEL
				: gphoto2::GP_CONTEXT_FEEDBACK_OK;
		}
		catch (...)
		{
			FILE_LOG(logERROR)
				<< "libgphoto2 question callback threw a C++ exception";
			return gphoto2::GP_CONTEXT_FEEDBACK_OK;
		}
	}

	gphoto2::GPContextFeedback ContextWrapper::cancelCallback(
		gphoto2::_GPContext *, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback = copyCallback(state->mutex, state->cancel);
		if (!callback)
		{
			return gphoto2::GP_CONTEXT_FEEDBACK_OK;
		}
		try
		{
			return callback() == ContextFeedback::Cancel
				? gphoto2::GP_CONTEXT_FEEDBACK_CANCEL
				: gphoto2::GP_CONTEXT_FEEDBACK_OK;
		}
		catch (...)
		{
			FILE_LOG(logERROR)
				<< "libgphoto2 cancel callback threw a C++ exception";
			return gphoto2::GP_CONTEXT_FEEDBACK_OK;
		}
	}

	unsigned int ContextWrapper::progressStartCallback(
		gphoto2::_GPContext *, float target, const char *text, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback =
			copyCallback(state->mutex, state->progressStart);
		if (!callback)
		{
			return 0;
		}
		try
		{
			return callback(target,
				text == nullptr ? std::string{} : std::string(text));
		}
		catch (...)
		{
			FILE_LOG(logERROR)
				<< "libgphoto2 progress start callback threw a C++ exception";
			return 0;
		}
	}

	void ContextWrapper::progressUpdateCallback(
		gphoto2::_GPContext *, unsigned int id, float current, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback =
			copyCallback(state->mutex, state->progressUpdate);
		if (callback)
		{
			try
			{
				callback(id, current);
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "libgphoto2 progress update callback threw a C++ exception";
			}
		}
	}

	void ContextWrapper::progressStopCallback(
		gphoto2::_GPContext *, unsigned int id, void *data)
	{
		auto *state = static_cast<ContextWrapper::State *>(data);
		const auto callback =
			copyCallback(state->mutex, state->progressStop);
		if (callback)
		{
			try
			{
				callback(id);
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "libgphoto2 progress stop callback threw a C++ exception";
			}
		}
	}

	ContextWrapper::ContextWrapper()
		: m_state(std::make_shared<State>())
		, m_context(gphoto2::gp_context_new())
	{
		if (m_context == nullptr)
		{
			throw std::runtime_error("Unable to create libgphoto2 context");
		}
	}

	ContextWrapper::~ContextWrapper()
	{
		if (m_context != nullptr)
		{
			gphoto2::gp_context_unref(m_context);
		}
	}

	ContextWrapper::ContextWrapper(ContextWrapper&& other) noexcept
		: m_state(std::move(other.m_state))
		, m_context(other.m_context)
	{
		other.m_context = nullptr;
	}

	ContextWrapper& ContextWrapper::operator=(
		ContextWrapper&& other) noexcept
	{
		if (this != &other)
		{
			if (m_context != nullptr)
			{
				gphoto2::gp_context_unref(m_context);
			}
			m_context = other.m_context;
			m_state = std::move(other.m_state);
			other.m_context = nullptr;
		}
		return *this;
	}

	gphoto2::_GPContext* ContextWrapper::getPtr() const
	{
		return m_context;
	}

	gphoto2::_GPContext* ContextWrapper::requireContext() const
	{
		if (m_context == nullptr)
		{
			throw std::logic_error("Context wrapper has been moved from");
		}
		return m_context;
	}

	void ContextWrapper::setIdleCallback(
		std::function<void()> callback)
	{
		auto *context = this->requireContext();
		{
			std::lock_guard<std::mutex> lock(m_state->mutex);
			m_state->idle = std::move(callback);
		}
		gphoto2::gp_context_set_idle_func(
			context, idleCallback, m_state.get());
	}

	void ContextWrapper::setErrorCallback(
		std::function<void(std::string const &)> callback)
	{
		auto *context = this->requireContext();
		{
			std::lock_guard<std::mutex> lock(m_state->mutex);
			m_state->error = std::move(callback);
		}
		gphoto2::gp_context_set_error_func(
			context, errorCallback, m_state.get());
	}

	void ContextWrapper::setStatusCallback(
		std::function<void(std::string const &)> callback)
	{
		auto *context = this->requireContext();
		{
			std::lock_guard<std::mutex> lock(m_state->mutex);
			m_state->status = std::move(callback);
		}
		gphoto2::gp_context_set_status_func(
			context, statusCallback, m_state.get());
	}

	void ContextWrapper::setMessageCallback(
		std::function<void(std::string const &)> callback)
	{
		auto *context = this->requireContext();
		{
			std::lock_guard<std::mutex> lock(m_state->mutex);
			m_state->message = std::move(callback);
		}
		gphoto2::gp_context_set_message_func(
			context, messageCallback, m_state.get());
	}

	void ContextWrapper::setQuestionCallback(
		std::function<ContextFeedback(std::string const &)> callback)
	{
		auto *context = this->requireContext();
		{
			std::lock_guard<std::mutex> lock(m_state->mutex);
			m_state->question = std::move(callback);
		}
		gphoto2::gp_context_set_question_func(
			context, questionCallback, m_state.get());
	}

	void ContextWrapper::setCancelCallback(
		std::function<ContextFeedback()> callback)
	{
		auto *context = this->requireContext();
		{
			std::lock_guard<std::mutex> lock(m_state->mutex);
			m_state->cancel = std::move(callback);
		}
		gphoto2::gp_context_set_cancel_func(
			context, cancelCallback, m_state.get());
	}

	void ContextWrapper::setProgressCallbacks(
		std::function<unsigned int(float, std::string const &)> start,
		std::function<void(unsigned int, float)> update,
		std::function<void(unsigned int)> stop)
	{
		auto *context = this->requireContext();
		{
			std::lock_guard<std::mutex> lock(m_state->mutex);
			m_state->progressStart = std::move(start);
			m_state->progressUpdate = std::move(update);
			m_state->progressStop = std::move(stop);
		}
		gphoto2::gp_context_set_progress_funcs(
			context, progressStartCallback, progressUpdateCallback,
			progressStopCallback, m_state.get());
	}

	void ContextWrapper::idle()
	{
		gphoto2::gp_context_idle(this->requireContext());
	}

	void ContextWrapper::reportError(std::string const & text)
	{
		if (text.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Context error text must not contain embedded NUL");
		}
		gphoto2::gp_context_error(
			this->requireContext(), "%s", text.c_str());
	}

	void ContextWrapper::reportStatus(std::string const & text)
	{
		if (text.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Context status text must not contain embedded NUL");
		}
		gphoto2::gp_context_status(
			this->requireContext(), "%s", text.c_str());
	}

	void ContextWrapper::reportMessage(std::string const & text)
	{
		if (text.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Context message text must not contain embedded NUL");
		}
		gphoto2::gp_context_message(
			this->requireContext(), "%s", text.c_str());
	}

	ContextFeedback ContextWrapper::ask(std::string const & text)
	{
		if (text.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Context question text must not contain embedded NUL");
		}
		const auto feedback = gphoto2::gp_context_question(
			this->requireContext(), "%s", text.c_str());
		return feedback == gphoto2::GP_CONTEXT_FEEDBACK_CANCEL
			? ContextFeedback::Cancel
			: ContextFeedback::Ok;
	}

	ContextFeedback ContextWrapper::cancelRequested()
	{
		const auto feedback = gphoto2::gp_context_cancel(
			this->requireContext());
		return feedback == gphoto2::GP_CONTEXT_FEEDBACK_CANCEL
			? ContextFeedback::Cancel
			: ContextFeedback::Ok;
	}

	unsigned int ContextWrapper::startProgress(
		float target, std::string const & text)
	{
		if (text.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Context progress text must not contain embedded NUL");
		}
		return gphoto2::gp_context_progress_start(
			this->requireContext(), target, "%s", text.c_str());
	}

	void ContextWrapper::updateProgress(unsigned int id, float current)
	{
		gphoto2::gp_context_progress_update(
			this->requireContext(), id, current);
	}

	void ContextWrapper::stopProgress(unsigned int id)
	{
		gphoto2::gp_context_progress_stop(
			this->requireContext(), id);
	}

}
