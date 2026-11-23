/** 
 * \file helper_gphoto2.hpp
 *
 * \author Copyright (c) 2013 maldworth <https://github.com/maldworth>
 * 
 * \author Copyright (c) 2026 Vitor Fonseca <https://github.com/VSF1>
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

#ifndef HELPERGPHOTO2_HPP
#define HELPERGPHOTO2_HPP

#include <string>
#include <vector>
#include <functional>

namespace gphoto2
{
	struct _GPContext;
}


namespace gphoto2pp
{
	class CameraListWrapper;
	
	/**
	 * \brief Finds the first available and recognized camera type connected to 
     * the computer
	 * \return the model and port of the first camera found
	 * \throw GPhoto2pp::exceptions::NoCameraFoundError if it didn't find any 
     * cameras
	 */
	std::pair<std::string, std::string> autoDetect();

	/**
	 * \brief Finds all attached and recognized camera types connected to the computer
	 * \return a list of all cameras found
	 * \throw GPhoto2pp::exceptions::NoCameraFoundError if it didn't find any cameras
	 */
	CameraListWrapper autoDetectAll();
	
	/**
	 * \brief Helper used to validate the response of gphoto2 methods. when GP_ error codes are < 0, this method will create a gphoto2_exception
	 * \param[in]	result	of the gphoto2 method
	 * \param[in]	methodName	of the gphoto2 method that was called
	 * \return the integer result returned from all gphoto2 methods. All results < 0 will be caught and thrown in the exception type. Some gphoto2 methods return the count of items, and so values > 0 can be returned and unrelated to error codes
	 * \throw GPhoto2pp::exceptions::gphoto2_exception
	 */
	int checkResponse(int result, std::string&& methodName);
	
	/**
	 * \brief Helper used to validate the response of gphoto2 methods, but does not throw an exception (logs the error instead)
	 * \param[in]	result	of the gphoto2 method
	 * \param[in]	methodName	of the gphoto2 method that was called
	 * \return the integer result returned from all gphoto2 methods. This is 
     * usually <= 0 (gphoto2 error codes), but some methods return the count of items, and so values > 0 can be returned and unrelated to error codes
	 */
	int checkResponseSilent(int result, std::string&& methodName);

	/**
	 * \brief Returns the version of gphoto library currently linked in the
     * system
	 * \param[in] verbose response, set to true, otherwise set to false
	 * \return An owning string with one newline-terminated line per entry.
	 * \note Direct wrapper for gp_library_version(...)
	 * \throw std::runtime_error if libgphoto2 returns a null array
	 */
	std::string LibraryVersion(bool verbose = false);

	/**
	 * \brief Returns the linked libgphoto2_port library version information.
	 * \param[in] verbose include build and backend details when true
	 * \return An owning string with one newline-terminated line per entry.
	 * \throw std::runtime_error if libgphoto2_port returns a null array
	 */
	std::string PortLibraryVersion(bool verbose = false);

	/**
	 * \brief Converts a libgphoto2_port result code to text.
	 * \param[in] result negative or positive port result code
	 * \return Owning result text
	 * \throw std::runtime_error if libgphoto2_port returns null
	 */
	std::string PortResultString(int result);

	/**
	 * \brief Sets or queries libgphoto2_port's gettext message codeset.
	 * \param[in] codeset codeset to set, without embedded NUL bytes
	 * \return The previously configured codeset.
	 * \throw gphoto2pp::exceptions::ArgumentException for embedded NUL bytes
	 * \throw std::runtime_error if libgphoto2_port returns no codeset string
	 */
	std::string PortMessageCodeset(std::string const & codeset);

	/**
	 * \brief Queries libgphoto2_port's current gettext message codeset.
	 * \return The current codeset.
	 * \throw std::runtime_error if libgphoto2_port returns no codeset string
	 */
	std::string PortMessageCodeset();

	/**
	 * \brief Sets libgphoto2's gettext message codeset.
	 * \param[in] codeset codeset to set, without embedded NUL bytes
	 * \return The resulting codeset reported by libgphoto2.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks this API
	 * \throw gphoto2pp::exceptions::ArgumentException for embedded NUL bytes
	 * \throw std::runtime_error if libgphoto2 returns no codeset string
	 */
	std::string MessageCodeset(std::string const & codeset);

	/**
	 * \brief Queries libgphoto2's current gettext message codeset.
	 * \return The current codeset.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks this API
	 * \throw std::runtime_error if libgphoto2 returns no codeset string
	 */
	std::string MessageCodeset();

	/**
	 * \brief Initializes libgphoto2's gettext locale directory.
	 * \param[in] directory root directory of localization files, without
	 * embedded NUL bytes
	 * \note Call before any non-initialization libgphoto2 function. The first
	 * initialization wins; later calls may be ignored.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks this API
	 * \throw gphoto2pp::exceptions::ArgumentException for embedded NUL bytes
	 * \throw gphoto2pp::exceptions::gphoto2_exception on library failure
	 */
	void InitializeLocaleDirectory(std::string const & directory);

	/**
	 * \brief Initializes libgphoto2's compiled-in gettext locale directory.
	 * \note Call before any non-initialization libgphoto2 function. The first
	 * initialization wins; later calls may be ignored.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks this API
	 * \throw gphoto2pp::exceptions::gphoto2_exception on library failure
	 */
	void InitializeLocaleDirectory();

	/**
	 * \brief Retrieves a persisted libgphoto2 setting.
	 * \param[in] id setting owner, at most 255 bytes and without embedded NUL
	 * \param[in] key setting key, at most 255 bytes and without embedded NUL
	 * \return The stored value.
	 * \note The C API has no output-capacity parameter. A custom getter
	 * registered outside gphoto2pp must write at most 255 bytes plus NUL.
	 * Setting storage is process-global; callers must serialize concurrent
	 * get and set operations.
	 * \throw gphoto2pp::exceptions::ArgumentException for invalid input
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks this API
	 * \throw gphoto2pp::exceptions::gphoto2_exception on library failure
	 * \throw std::runtime_error if the getter does not NUL-terminate its result
	 */
	std::string GetSetting(std::string const & id, std::string const & key);

	/**
	 * \brief Stores a persisted libgphoto2 setting.
	 * \param[in] id setting owner, at most 255 bytes and without embedded NUL
	 * \param[in] key setting key, at most 255 bytes and without embedded NUL
	 * \param[in] value setting value, at most 255 bytes and without embedded NUL
	 * \note Setting storage is process-global; callers must serialize
	 * concurrent get and set operations.
	 * \throw gphoto2pp::exceptions::ArgumentException for invalid input
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks this API
	 * \throw gphoto2pp::exceptions::gphoto2_exception on library failure
	 */
	void SetSetting(std::string const & id, std::string const & key,
		std::string const & value);

	/**
	 * \brief C++ callback for retrieving a persisted libgphoto2 setting.
	 * \param[in] id setting owner
	 * \param[in] key setting key
	 * \param[out] value stored value, at most 255 bytes on success
	 * \return A libgphoto2 result code.
	 */
	using SettingGetCallback = std::function<int(
		std::string const &, std::string const &, std::string &)>;

	/**
	 * \brief C++ callback for storing a persisted libgphoto2 setting.
	 * \param[in] id setting owner
	 * \param[in] key setting key
	 * \param[in] value setting value
	 * \return A libgphoto2 result code.
	 */
	using SettingSetCallback = std::function<int(
		std::string const &, std::string const &, std::string const &)>;

	/**
	 * \brief Installs or clears libgphoto2's process-global settings getter.
	 * \param[in] callback callback to install, or empty to clear
	 * \note Configure before camera operations and do not change while setting
	 * callbacks are in flight. Callback bodies may run concurrently and must
	 * provide their own synchronization. The getter output is limited to 255
	 * bytes to fit libgphoto2's 256-byte C callback convention.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks callback registration APIs
	 */
	void SetSettingGetCallback(SettingGetCallback callback);

	/**
	 * \brief Installs or clears libgphoto2's process-global settings setter.
	 * \param[in] callback callback to install, or empty to clear
	 * \note Configure before camera operations and do not change while setting
	 * callbacks are in flight. Callback bodies may run concurrently and must
	 * provide their own synchronization.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2 lacks callback registration APIs
	 */
	void SetSettingSetCallback(SettingSetCallback callback);

	/**
	 * \brief Initializes libgphoto2_port's gettext locale directory.
	 * \note Call before any other libgphoto2_port initialization. The first
	 * initialization wins; later calls may be ignored by the library.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2_port lacks this API
	 * \throw gphoto2pp::exceptions::ArgumentException for embedded NUL bytes
	 * in the directory
	 * \throw gphoto2pp::exceptions::gphoto2_exception on library failure
	 */
	void InitializePortLocaleDirectory(std::string const & directory);

	/**
	 * \brief Initializes libgphoto2_port's compiled-in locale directory.
	 * \note Call before any other libgphoto2_port initialization. The first
	 * initialization wins; later calls may be ignored by the library.
	 * \throw gphoto2pp::exceptions::InvalidLinkedVersionException when the
	 * linked libgphoto2_port lacks this API
	 * \throw gphoto2pp::exceptions::gphoto2_exception on library failure
	 */
	void InitializePortLocaleDirectory();
}

#endif // HELPERGPHOTO2_HPP
