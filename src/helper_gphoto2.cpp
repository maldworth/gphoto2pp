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

#include <gphoto2pp/helper_gphoto2.hpp>

#include <gphoto2pp/log.h>

#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>

#ifdef GPHOTO_LESS_25
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>
#include <gphoto2pp/camera_abilities_list_wrapper.hpp>
#else
#include <gphoto2pp/helper_context.hpp>
#endif

namespace gphoto2
{
#ifndef GPHOTO_LESS_25
#include <gphoto2/gphoto2-camera.h> // Needed for gp_camera_autodetect, but only used if gphoto is 2.5 or greater
#else
#include <gphoto2/gphoto2-result.h> // used for gp_result_as_string
#include <gphoto2/gphoto2-context.h>
#endif
#include <gphoto2/gphoto2-abilities-list.h>
#include <gphoto2/gphoto2-setting.h>
#include <gphoto2/gphoto2-version.h>
#include <gphoto2/gphoto2-port-result.h>
#include <gphoto2/gphoto2-port-info-list.h>
}

#include <sstream>
#include <array>
#include <cstddef>
#include <cstring>
#include <mutex>
#include <new>
#include <stdexcept>
#include <utility>
#include <vector>

namespace gphoto2pp
{
	namespace
	{
		constexpr std::size_t maxSettingFieldLength = 255;

		struct SettingCallbacksState
		{
			std::mutex m_callbackMutex;
			std::mutex m_registrationMutex;
			SettingGetCallback m_get;
			SettingSetCallback m_set;
		};

		SettingCallbacksState & settingCallbacksState()
		{
			static SettingCallbacksState state;
			return state;
		}

		std::string formatLibraryVersion(char const *const *lines)
		{
			if (lines == nullptr)
			{
				throw std::runtime_error("Library returned a null version array");
			}
			std::ostringstream version;
			for (auto line = lines; *line != nullptr; ++line)
			{
				version << *line << '\n';
			}
			return version.str();
		}

		std::vector<char> makeSettingCString(
			std::string const & value, char const *fieldName)
		{
			if (value.find('\0') != std::string::npos ||
				value.size() > maxSettingFieldLength)
			{
				throw exceptions::ArgumentException(
					std::string("Setting ") + fieldName +
					" must not contain embedded NUL bytes or exceed 255 bytes");
			}
			std::vector<char> buffer(value.begin(), value.end());
			buffer.push_back('\0');
			return buffer;
		}

		int settingGetCallbackBridge(
			char *id, char *key, char *value, void *)
		{
			if (id == nullptr || key == nullptr || value == nullptr)
			{
				return GP_ERROR_BAD_PARAMETERS;
			}

			SettingGetCallback callback;
			try
			{
				auto & state = settingCallbacksState();
				std::lock_guard<std::mutex> lock{state.m_callbackMutex};
				callback = state.m_get;
			}
			catch (const std::bad_alloc &)
			{
				FILE_LOG(logERROR)
					<< "Could not copy libgphoto2 setting getter callback";
				return GP_ERROR_NO_MEMORY;
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "Could not copy libgphoto2 setting getter callback";
				return GP_ERROR;
			}
			if (!callback)
			{
				return GP_ERROR_BAD_PARAMETERS;
			}

			try
			{
				std::string settingValue;
				const int result = callback(id, key, settingValue);
				if (result < GP_OK)
				{
					return result;
				}
				const auto output = makeSettingCString(
					settingValue, "callback value");
				std::memcpy(value, output.data(), output.size());
				return result;
			}
			catch (const exceptions::ArgumentException &)
			{
				FILE_LOG(logERROR)
					<< "Libgphoto2 setting getter returned an invalid value";
				return GP_ERROR_BAD_PARAMETERS;
			}
			catch (const std::bad_alloc &)
			{
				FILE_LOG(logERROR)
					<< "Libgphoto2 setting getter ran out of memory";
				return GP_ERROR_NO_MEMORY;
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "Libgphoto2 setting getter callback threw";
				return GP_ERROR;
			}
		}

		int settingSetCallbackBridge(
			char *id, char *key, char *value, void *)
		{
			if (id == nullptr || key == nullptr || value == nullptr)
			{
				return GP_ERROR_BAD_PARAMETERS;
			}

			SettingSetCallback callback;
			std::string idValue;
			std::string keyValue;
			std::string settingValue;
			try
			{
				idValue = id;
				keyValue = key;
				settingValue = value;
				if (idValue.size() > maxSettingFieldLength ||
					keyValue.size() > maxSettingFieldLength ||
					settingValue.size() > maxSettingFieldLength)
				{
					return GP_ERROR_BAD_PARAMETERS;
				}

				auto & state = settingCallbacksState();
				std::lock_guard<std::mutex> lock{state.m_callbackMutex};
				callback = state.m_set;
			}
			catch (const std::bad_alloc &)
			{
				FILE_LOG(logERROR)
					<< "Could not copy libgphoto2 setting setter callback";
				return GP_ERROR_NO_MEMORY;
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "Could not prepare libgphoto2 setting setter callback";
				return GP_ERROR;
			}
			if (!callback)
			{
				return GP_ERROR_BAD_PARAMETERS;
			}

			try
			{
				return callback(idValue, keyValue, settingValue);
			}
			catch (const std::bad_alloc &)
			{
				FILE_LOG(logERROR)
					<< "Libgphoto2 setting setter ran out of memory";
				return GP_ERROR_NO_MEMORY;
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "Libgphoto2 setting setter callback threw";
				return GP_ERROR;
			}
		}
	}

	bool exceptions::gphoto2_exception::isNotSupported() const
	{
		return this->getResultCode() == GP_ERROR_NOT_SUPPORTED;
	}

	std::pair<std::string, std::string> autoDetect()
	{
		auto cameraList = autoDetectAll();
		
		return cameraList.getPair(0);
	}

	CameraListWrapper autoDetectAll()
	{
		CameraListWrapper cameraListWrapper;
#ifdef GPHOTO_LESS_25
		//All this logic was added to the gp_camera_autodetect method in 2.5 or greater.
		
		// Constructor for this class will load all port drivers
		GPPortInfoListWrapper portInfoListWrapper;
		
		// Loads all the camera drivers we have
		CameraAbilitiesListWrapper cameraAbilitiesListWrapper;
		
		auto tempListWrapper = cameraAbilitiesListWrapper.listDetect(portInfoListWrapper);
		
		//Now we need to filter out the usb:
		for (int i = 0; i < tempListWrapper.count(); i++)
		{
			auto name = tempListWrapper.getName(i);
			auto value = tempListWrapper.getValue(i);
			
			if(!std::string("usb:").compare(value))
			{
				// We don't want to capture items with usb:
				continue;
			}
			cameraListWrapper.append(name, value);
		}
#else
		ContextWrapper context;
		gphoto2pp::checkResponse(gphoto2::gp_camera_autodetect(
			cameraListWrapper.getPtr(), context.getPtr()),
			"gp_camera_autodetect");
#endif
		int count = cameraListWrapper.count();
		
		if(count == 0)
		{
			throw exceptions::NoCameraFoundError("autoDetect(multiple)");
		}
		
		// return std::move(cameraListWrapper);
		return cameraListWrapper;
	}
	
	int checkResponse(int result, std::string&& methodName)
	{
		if(result < 0) 
		{
			std::stringstream errorMessage;
			errorMessage << methodName << ": failed with return code '" << result << "' and the reason is: '" << gphoto2::gp_result_as_string(result) << "'";
			FILE_LOG(logERROR) << "Exception Message - '"<< errorMessage.str().c_str() << "'";
			throw exceptions::gphoto2_exception(result, errorMessage.str());
		}
		return result;
	}

	int checkResponseSilent(int result, std::string&& methodName)
	{
		try
		{
			gphoto2pp::checkResponse(result, std::move(methodName));
		}
		catch(const exceptions::gphoto2_exception& e)
		{
			FILE_LOG(logERROR) << "Suppressed gphoto2_exception[" << e.what() <<"]";
		}
		catch(const std::runtime_error& e)
		{
			FILE_LOG(logCRITICAL) << "Suppressed runtime_error[" << e.what() <<"]";
		}
		catch(const std::exception& e)
		{
			FILE_LOG(logEMERGENCY) << "Suppressed std::exception";
		}
		return result;
	}

	std::string LibraryVersion(bool verbose)
	{
		const auto verbosity = verbose
			? gphoto2::GP_VERSION_VERBOSE : gphoto2::GP_VERSION_SHORT;
		return formatLibraryVersion(gphoto2::gp_library_version(verbosity));
	}

	std::string PortLibraryVersion(bool verbose)
	{
		const auto verbosity = verbose
			? gphoto2::GP_VERSION_VERBOSE : gphoto2::GP_VERSION_SHORT;
		return formatLibraryVersion(gphoto2::gp_port_library_version(verbosity));
	}

	std::string PortResultString(int result)
	{
		const char *text =
			gphoto2::gp_port_result_as_string(result);
		if (text == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2_port returned a null result string");
		}
		return std::string(text);
	}

	std::string PortMessageCodeset(std::string const & codeset)
	{
		if (codeset.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Port message codeset must not contain embedded NUL bytes");
		}
		const char *previous =
			gphoto2::gp_port_message_codeset(codeset.c_str());
		if (previous == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2_port returned a null message codeset");
		}
		return std::string(previous);
	}

	std::string PortMessageCodeset()
	{
		const char *current = gphoto2::gp_port_message_codeset(nullptr);
		if (current == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2_port returned a null message codeset");
		}
		return std::string(current);
	}

	std::string MessageCodeset(std::string const & codeset)
	{
		if (codeset.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Message codeset must not contain embedded NUL bytes");
		}
#ifdef GPHOTO2PP_HAVE_MESSAGE_CODESET
		const char *result =
			gphoto2::gp_message_codeset(codeset.c_str());
		if (result == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2 returned a null message codeset");
		}
		return std::string(result);
#else
		throw exceptions::InvalidLinkedVersionException(
			"Setting the message codeset requires gp_message_codeset");
#endif
	}

	std::string MessageCodeset()
	{
#ifdef GPHOTO2PP_HAVE_MESSAGE_CODESET
		const char *current = gphoto2::gp_message_codeset(nullptr);
		if (current == nullptr)
		{
			throw std::runtime_error(
				"libgphoto2 returned a null message codeset");
		}
		return std::string(current);
#else
		throw exceptions::InvalidLinkedVersionException(
			"Querying the message codeset requires gp_message_codeset");
#endif
	}

	void InitializeLocaleDirectory(std::string const & directory)
	{
		if (directory.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Locale directory must not contain embedded NUL bytes");
		}
#ifdef GPHOTO2PP_HAVE_INIT_LOCALEDIR
		gphoto2pp::checkResponse(
			gphoto2::gp_init_localedir(directory.c_str()),
			"gp_init_localedir");
#else
		throw exceptions::InvalidLinkedVersionException(
			"Initializing the locale directory requires "
			"gp_init_localedir (libgphoto2 2.5.31 or greater)");
#endif
	}

	void InitializeLocaleDirectory()
	{
#ifdef GPHOTO2PP_HAVE_INIT_LOCALEDIR
		gphoto2pp::checkResponse(
			gphoto2::gp_init_localedir(nullptr), "gp_init_localedir");
#else
		throw exceptions::InvalidLinkedVersionException(
			"Initializing the locale directory requires "
			"gp_init_localedir (libgphoto2 2.5.31 or greater)");
#endif
	}

	std::string GetSetting(
		std::string const & id, std::string const & key)
	{
		auto idBuffer = makeSettingCString(id, "id");
		auto keyBuffer = makeSettingCString(key, "key");
#ifdef GPHOTO2PP_HAVE_SETTING_GET
		std::array<char, maxSettingFieldLength + 1> valueBuffer{};
		gphoto2pp::checkResponse(
			gphoto2::gp_setting_get(
				idBuffer.data(), keyBuffer.data(), valueBuffer.data()),
			"gp_setting_get");
		if (std::memchr(valueBuffer.data(), '\0', valueBuffer.size()) == nullptr)
		{
			throw std::runtime_error(
				"gp_setting_get returned an unterminated value");
		}
		return std::string(valueBuffer.data());
#else
		throw exceptions::InvalidLinkedVersionException(
			"Retrieving settings requires gp_setting_get");
#endif
	}

	void SetSetting(std::string const & id, std::string const & key,
		std::string const & value)
	{
		auto idBuffer = makeSettingCString(id, "id");
		auto keyBuffer = makeSettingCString(key, "key");
		auto valueBuffer = makeSettingCString(value, "value");
#ifdef GPHOTO2PP_HAVE_SETTING_SET
		gphoto2pp::checkResponse(
			gphoto2::gp_setting_set(
				idBuffer.data(), keyBuffer.data(), valueBuffer.data()),
			"gp_setting_set");
#else
		throw exceptions::InvalidLinkedVersionException(
			"Storing settings requires gp_setting_set");
#endif
	}

	void SetSettingGetCallback(SettingGetCallback callback)
	{
#ifdef GPHOTO2PP_HAVE_SETTING_CALLBACKS
		auto & state = settingCallbacksState();
		std::lock_guard<std::mutex> registrationLock{
			state.m_registrationMutex};
		if (callback)
		{
			{
				std::lock_guard<std::mutex> callbackLock{
					state.m_callbackMutex};
				state.m_get = std::move(callback);
			}
			gphoto2::gp_setting_set_get_func(
				settingGetCallbackBridge, nullptr);
		}
		else
		{
			gphoto2::gp_setting_set_get_func(nullptr, nullptr);
			std::lock_guard<std::mutex> callbackLock{
				state.m_callbackMutex};
			state.m_get = {};
		}
#else
		(void)callback;
		throw exceptions::InvalidLinkedVersionException(
			"Setting getter callbacks require "
			"gp_setting_set_get_func and gp_setting_set_set_func");
#endif
	}

	void SetSettingSetCallback(SettingSetCallback callback)
	{
#ifdef GPHOTO2PP_HAVE_SETTING_CALLBACKS
		auto & state = settingCallbacksState();
		std::lock_guard<std::mutex> registrationLock{
			state.m_registrationMutex};
		if (callback)
		{
			{
				std::lock_guard<std::mutex> callbackLock{
					state.m_callbackMutex};
				state.m_set = std::move(callback);
			}
			gphoto2::gp_setting_set_set_func(
				settingSetCallbackBridge, nullptr);
		}
		else
		{
			gphoto2::gp_setting_set_set_func(nullptr, nullptr);
			std::lock_guard<std::mutex> callbackLock{
				state.m_callbackMutex};
			state.m_set = {};
		}
#else
		(void)callback;
		throw exceptions::InvalidLinkedVersionException(
			"Setting setter callbacks require "
			"gp_setting_set_get_func and gp_setting_set_set_func");
#endif
	}

	void InitializePortLocaleDirectory(std::string const & directory)
	{
		if (directory.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Port locale directory must not contain embedded NUL bytes");
		}
#ifdef GPHOTO2PP_HAVE_PORT_INIT_LOCALEDIR
		gphoto2pp::checkResponse(
			gphoto2::gp_port_init_localedir(directory.c_str()),
			"gp_port_init_localedir");
#else
		throw exceptions::InvalidLinkedVersionException(
			"Initializing the port locale directory requires "
			"gp_port_init_localedir (libgphoto2_port 0.12.1 or greater)");
#endif
	}

	void InitializePortLocaleDirectory()
	{
#ifdef GPHOTO2PP_HAVE_PORT_INIT_LOCALEDIR
		gphoto2pp::checkResponse(
			gphoto2::gp_port_init_localedir(nullptr),
			"gp_port_init_localedir");
#else
		throw exceptions::InvalidLinkedVersionException(
			"Initializing the port locale directory requires "
			"gp_port_init_localedir (libgphoto2_port 0.12.1 or greater)");
#endif
	}
}
