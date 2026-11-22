/**
 * \file camera_wrapper.cpp
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

#include <gphoto2pp/camera_wrapper.hpp>

#include <gphoto2pp/exceptions.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/helper_context.hpp>
#include <gphoto2pp/window_widget.hpp>
#include <gphoto2pp/non_value_widget.hpp>
#include <gphoto2pp/text_widget.hpp>
#include <gphoto2pp/range_widget.hpp>
#include <gphoto2pp/float_widget.hpp>
#include <gphoto2pp/int_widget.hpp>
#include <gphoto2pp/string_widget.hpp>
#include <gphoto2pp/toggle_widget.hpp>
#include <gphoto2pp/radio_widget.hpp>
#include <gphoto2pp/menu_widget.hpp>
#include <gphoto2pp/date_widget.hpp>
#include <gphoto2pp/button_widget.hpp>
#include <gphoto2pp/camera_widget_type_wrapper.hpp>
#include <gphoto2pp/camera_abilities_list_wrapper.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/gp_port_info_list_wrapper.hpp>

#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/camera_file_path_wrapper.hpp>
#include <gphoto2pp/camera_event_type_wrapper.hpp>
#include <gphoto2pp/camera_capture_type_wrapper.hpp>

#include <gphoto2pp/log.h>

namespace gphoto2
{
#include <gphoto2/gphoto2-camera.h>
#ifndef GPHOTO_LESS_25
#include <gphoto2/gphoto2-abilities-list.h> // Only needed for the pre 2.5 initialize method (because _autodetect doesn't exist)
#endif
#include <gphoto2/gphoto2-file.h>
}

#include <fstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>
#include <chrono>
#include <condition_variable>
#include <climits>
#include <exception>
#include <unordered_set>

namespace gphoto2pp
{
	namespace detail
	{
		struct CameraTimeoutCallbackState
			: std::enable_shared_from_this<CameraTimeoutCallbackState>
		{
			explicit CameraTimeoutCallbackState(
				CameraTimeoutCallbacks value)
				: callbacks{std::move(value)}
			{
			}

			void activate(gphoto2::Camera *cameraValue,
				gphoto2::GPContext *contextValue)
			{
				std::lock_guard<std::mutex> lock{mutex};
				camera = cameraValue;
				context = contextValue;
				active = true;
			}

			void deactivate()
			{
				std::unique_lock<std::mutex> lock{mutex};
				active = false;
				callbacksDrained.wait(lock, [this]() {
					return startsInFlight == 0 && callbacksInFlight == 0;
				});
				camera = nullptr;
				context = nullptr;
				activeTimerIds.clear();
			}

			void deactivateIfIdle()
			{
				std::unique_lock<std::mutex> lock{mutex};
				if (!activeTimerIds.empty() || startsInFlight != 0)
				{
					throw exceptions::CameraWrapperException(
						"Cannot remove timeout callbacks while timers are active");
				}
				active = false;
				callbacksDrained.wait(lock, [this]() {
					return callbacksInFlight == 0;
				});
				camera = nullptr;
				context = nullptr;
			}

			bool hasActiveTimers() const
			{
				std::lock_guard<std::mutex> lock{mutex};
				return !activeTimerIds.empty();
			}

			void replaceCallbacks(CameraTimeoutCallbacks value)
			{
				std::lock_guard<std::mutex> lock{mutex};
				if (!activeTimerIds.empty() || startsInFlight != 0 ||
					callbacksInFlight != 0)
				{
					throw exceptions::CameraWrapperException(
						"Cannot replace timeout callbacks while timers are active");
				}
				callbacks = std::move(value);
			}

			unsigned int start(unsigned int timeout,
				gphoto2::CameraTimeoutFunc function)
			{
				CameraTimeoutCallbacks currentCallbacks;
				{
					std::lock_guard<std::mutex> lock{mutex};
					if (!active || !callbacks.start || !callbacks.stop)
					{
						return static_cast<unsigned int>(GP_ERROR);
					}
					currentCallbacks = callbacks;
					++startsInFlight;
				}

				const auto finishStart = [this]() {
					std::lock_guard<std::mutex> lock{mutex};
					--startsInFlight;
					if (startsInFlight == 0 && callbacksInFlight == 0)
					{
						callbacksDrained.notify_all();
					}
				};
				unsigned int id = 0;
				try
				{
					auto self = shared_from_this();
					CameraTimeoutAction action = [self, function]() {
						return self->invoke(function);
					};
					id = currentCallbacks.start(
						timeout, std::move(action));
				}
				catch (...)
				{
					finishStart();
					throw;
				}
				if (id > static_cast<unsigned int>(INT_MAX))
				{
					try
					{
						currentCallbacks.stop(id);
					}
					catch (...)
					{
						FILE_LOG(logERROR)
							<< "Timeout scheduler failed to stop an "
							<< "unrepresentable timer ID";
					}
					finishStart();
					return static_cast<unsigned int>(GP_ERROR);
				}

				try
				{
					std::lock_guard<std::mutex> lock{mutex};
					if (active)
					{
						activeTimerIds.insert(id);
						--startsInFlight;
						if (startsInFlight == 0 && callbacksInFlight == 0)
						{
							callbacksDrained.notify_all();
						}
						return id;
					}
				}
				catch (...)
				{
					try
					{
						currentCallbacks.stop(id);
					}
					catch (...)
					{
						FILE_LOG(logERROR)
							<< "Timeout scheduler failed to stop a timer "
							<< "that could not be tracked";
					}
					finishStart();
					throw;
				}

				try
				{
					currentCallbacks.stop(id);
				}
				catch (...)
				{
					FILE_LOG(logERROR)
						<< "Timeout scheduler failed to stop a timer "
						<< "created during camera shutdown";
				}
				finishStart();
				return static_cast<unsigned int>(GP_ERROR);
			}

			void stop(unsigned int id) noexcept
			{
				CameraTimeoutCallbacks currentCallbacks;
				{
					std::lock_guard<std::mutex> lock{mutex};
					currentCallbacks = callbacks;
				}

				try
				{
					if (currentCallbacks.stop)
					{
						currentCallbacks.stop(id);
					}
				}
				catch (const std::exception &error)
				{
					FILE_LOG(logERROR)
						<< "Timeout scheduler stop callback failed: "
						<< error.what();
				}
				catch (...)
				{
					FILE_LOG(logERROR)
						<< "Timeout scheduler stop callback failed";
				}

				std::lock_guard<std::mutex> lock{mutex};
				activeTimerIds.erase(id);
			}

			int invoke(gphoto2::CameraTimeoutFunc function) noexcept
			{
				gphoto2::Camera *cameraValue = nullptr;
				gphoto2::GPContext *contextValue = nullptr;
				{
					std::lock_guard<std::mutex> lock{mutex};
					if (!active || function == nullptr)
					{
						return GP_ERROR_CANCEL;
					}
					cameraValue = camera;
					contextValue = context;
					++callbacksInFlight;
				}

				int result = GP_ERROR;
				try
				{
					result = function(cameraValue, contextValue);
				}
				catch (const std::exception &error)
				{
					FILE_LOG(logERROR)
						<< "Camera timeout action threw an exception: "
						<< error.what();
				}
				catch (...)
				{
					FILE_LOG(logERROR)
						<< "Camera timeout action threw an exception";
				}

				{
					std::lock_guard<std::mutex> lock{mutex};
					--callbacksInFlight;
					if (callbacksInFlight == 0)
					{
						callbacksDrained.notify_all();
					}
				}
				return result;
			}

			CameraTimeoutCallbacks callbacks;
			mutable std::mutex mutex;
			std::condition_variable callbacksDrained;
			gphoto2::Camera *camera = nullptr;
			gphoto2::GPContext *context = nullptr;
			std::unordered_set<unsigned int> activeTimerIds;
			std::size_t startsInFlight = 0;
			std::size_t callbacksInFlight = 0;
			bool active = false;
		};
	}

	namespace
	{
		void validateTimeoutCallbacks(
			CameraTimeoutCallbacks const & callbacks)
		{
			if (static_cast<bool>(callbacks.start) !=
				static_cast<bool>(callbacks.stop))
			{
				throw exceptions::ArgumentException(
					"Both camera timeout callbacks must be set together");
			}
		}

		std::shared_ptr<detail::CameraTimeoutCallbackState>
		makeTimeoutCallbackState(
			CameraTimeoutCallbacks const & callbacks)
		{
			validateTimeoutCallbacks(callbacks);
			if (!callbacks.start)
			{
				return {};
			}
			return std::make_shared<detail::CameraTimeoutCallbackState>(
				callbacks);
		}

		unsigned int cameraTimeoutStart(
			gphoto2::Camera *camera, unsigned int timeout,
			gphoto2::CameraTimeoutFunc function, void *data)
		{
			(void)camera;
			if (data == nullptr || function == nullptr)
			{
				return static_cast<unsigned int>(
					GP_ERROR_BAD_PARAMETERS);
			}
			try
			{
				return static_cast<detail::CameraTimeoutCallbackState *>(data)
					->start(timeout, function);
			}
			catch (const std::exception &error)
			{
				FILE_LOG(logERROR)
					<< "Camera timeout start callback failed: "
					<< error.what();
			}
			catch (...)
			{
				FILE_LOG(logERROR)
					<< "Camera timeout start callback failed";
			}
			return static_cast<unsigned int>(GP_ERROR);
		}

		void cameraTimeoutStop(
			gphoto2::Camera *camera, unsigned int id, void *data)
		{
			(void)camera;
			if (data != nullptr)
			{
				static_cast<detail::CameraTimeoutCallbackState *>(data)
					->stop(id);
			}
		}

		std::string cameraTextToString(gphoto2::CameraText const & text)
		{
			const char *end = static_cast<const char *>(
				std::memchr(text.text, '\0', sizeof(text.text)));
			if (end == nullptr)
			{
				throw std::runtime_error(
					"Camera returned unterminated text");
			}
			return std::string(text.text, end);
		}

		template<std::size_t N>
		std::string metadataString(const char (&value)[N], const char *description)
		{
			const auto length = ::strnlen(value, N);
			if (length == N)
			{
				throw std::runtime_error(std::string("Camera ") + description + " is not terminated");
			}
			return std::string(value, length);
		}

		template<typename T>
		CameraFileInfoPart fileInfoPart(const T &source)
		{
			CameraFileInfoPart part;
			part.fields = static_cast<std::uint32_t>(source.fields);
			if (part.has(CameraFileInfoField::Status))
			{
				part.status = static_cast<CameraFileStatus>(source.status);
			}
			if (part.has(CameraFileInfoField::Size))
			{
				part.size = source.size;
			}
			if (part.has(CameraFileInfoField::Type))
			{
				part.mimeType = metadataString(source.type, "file MIME type");
			}
			return part;
		}

		void findWidgetsByName(gphoto2::_CameraWidget *widget,
			std::string const & name,
			std::vector<gphoto2::_CameraWidget *> &matches)
		{
			const char *widgetName = nullptr;
			gphoto2pp::checkResponse(
				gphoto2::gp_widget_get_name(widget, &widgetName),
				"gp_widget_get_name");
			if (widgetName != nullptr && name == widgetName)
			{
				matches.push_back(widget);
			}

			const int childCount = gphoto2pp::checkResponse(
				gphoto2::gp_widget_count_children(widget),
				"gp_widget_count_children");
			for (int index = 0; index < childCount; ++index)
			{
				gphoto2::_CameraWidget *child = nullptr;
				gphoto2pp::checkResponse(
					gphoto2::gp_widget_get_child(widget, index, &child),
					"gp_widget_get_child");
				findWidgetsByName(child, name, matches);
			}
		}

#if defined(GPHOTO2PP_HAVE_CAMERA_GET_SINGLE_CONFIG) && \
	defined(GPHOTO2PP_HAVE_CAMERA_SET_SINGLE_CONFIG)
		void copyConfigValue(CameraWidgetWrapper & destination,
			CameraWidgetWrapper const & source, CameraWidgetTypeWrapper type)
		{
			switch (type)
			{
				case CameraWidgetTypeWrapper::Text:
				{
					auto *target = dynamic_cast<TextWidget *>(&destination);
					auto const *value = dynamic_cast<TextWidget const *>(&source);
					std::string text;
					if (target == nullptr || value == nullptr)
					{
						throw exceptions::InvalidWidgetType(
							"Configuration text widget has an invalid wrapper type");
					}
					if (!value->tryGetValue(text))
					{
						throw exceptions::NullWidget(
							"Configuration text widget has no value");
					}
					target->setValue(text);
					return;
				}
				case CameraWidgetTypeWrapper::Range:
				{
					auto *target = dynamic_cast<RangeWidget *>(&destination);
					auto const *value = dynamic_cast<RangeWidget const *>(&source);
					if (target == nullptr || value == nullptr)
					{
						throw exceptions::InvalidWidgetType(
							"Configuration range widget has an invalid wrapper type");
					}
					target->setValue(value->getValue());
					return;
				}
				case CameraWidgetTypeWrapper::Toggle:
				{
					auto *target = dynamic_cast<ToggleWidget *>(&destination);
					auto const *value = dynamic_cast<ToggleWidget const *>(&source);
					if (target == nullptr || value == nullptr)
					{
						throw exceptions::InvalidWidgetType(
							"Configuration toggle widget has an invalid wrapper type");
					}
					target->setValue(value->getValue());
					return;
				}
				case CameraWidgetTypeWrapper::Radio:
				{
					auto *target = dynamic_cast<RadioWidget *>(&destination);
					auto const *value = dynamic_cast<RadioWidget const *>(&source);
					if (target == nullptr || value == nullptr)
					{
						throw exceptions::InvalidWidgetType(
							"Configuration radio widget has an invalid wrapper type");
					}
					target->setValue(value->getValue());
					return;
				}
				case CameraWidgetTypeWrapper::Menu:
				{
					auto *target = dynamic_cast<MenuWidget *>(&destination);
					auto const *value = dynamic_cast<MenuWidget const *>(&source);
					if (target == nullptr || value == nullptr)
					{
						throw exceptions::InvalidWidgetType(
							"Configuration menu widget has an invalid wrapper type");
					}
					target->setValue(value->getValue());
					return;
				}
				case CameraWidgetTypeWrapper::Date:
				{
					auto *target = dynamic_cast<DateWidget *>(&destination);
					auto const *value = dynamic_cast<DateWidget const *>(&source);
					if (target == nullptr || value == nullptr)
					{
						throw exceptions::InvalidWidgetType(
							"Configuration date widget has an invalid wrapper type");
					}
					const std::time_t date = value->getValue();
					if (date < std::numeric_limits<int>::min() ||
						date > std::numeric_limits<int>::max())
					{
						throw exceptions::ArgumentException(
							"Configuration date value is outside libgphoto2's range");
					}
					target->setValue(date);
					return;
				}
				default:
					throw exceptions::InvalidWidgetType(
						"Configuration widget type does not contain a writable value");
			}
		}
#endif
	}

	CameraAbilitiesSnapshot cameraAbilitiesToSnapshot(
		gphoto2::CameraAbilities const & source)
	{
		CameraAbilitiesSnapshot snapshot;
		snapshot.model = metadataString(source.model, "camera abilities model");
		snapshot.status = static_cast<CameraDriverStatus>(source.status);
		snapshot.port = static_cast<std::uint32_t>(source.port);
		for (int speed : source.speed)
		{
			if (speed == 0)
			{
				break;
			}
			snapshot.speeds.push_back(speed);
		}
		snapshot.operations =
			static_cast<std::uint32_t>(source.operations);
		snapshot.fileOperations =
			static_cast<std::uint32_t>(source.file_operations);
		snapshot.folderOperations =
			static_cast<std::uint32_t>(source.folder_operations);
		snapshot.usbVendor = source.usb_vendor;
		snapshot.usbProduct = source.usb_product;
		snapshot.usbClass = source.usb_class;
		snapshot.usbSubclass = source.usb_subclass;
		snapshot.usbProtocol = source.usb_protocol;
		snapshot.library = metadataString(
			source.library, "camera abilities library");
		snapshot.id = metadataString(source.id, "camera abilities id");
		snapshot.deviceType =
			static_cast<GphotoDeviceType>(source.device_type);
		return snapshot;
	}

	gphoto2::CameraAbilities snapshotToCameraAbilities(
		CameraAbilitiesSnapshot const & snapshot)
	{
		if (snapshot.model.find('\0') != std::string::npos ||
			snapshot.library.find('\0') != std::string::npos ||
			snapshot.id.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera abilities strings must not contain embedded NUL");
		}
		if (snapshot.model.size() >= sizeof(gphoto2::CameraAbilities::model) ||
			snapshot.library.size() >= sizeof(gphoto2::CameraAbilities::library) ||
			snapshot.id.size() >= sizeof(gphoto2::CameraAbilities::id) ||
			snapshot.speeds.size() >=
				sizeof(gphoto2::CameraAbilities::speed) / sizeof(int))
		{
			throw exceptions::ArgumentException(
				"Camera abilities snapshot exceeds libgphoto2 fixed limits");
		}
		gphoto2::CameraAbilities result{};
		std::memcpy(result.model, snapshot.model.c_str(),
			snapshot.model.size());
		result.status = static_cast<gphoto2::CameraDriverStatus>(
			snapshot.status);
		result.port = static_cast<gphoto2::GPPortType>(snapshot.port);
		for (std::size_t i = 0; i < snapshot.speeds.size(); ++i)
		{
			result.speed[i] = snapshot.speeds[i];
		}
		result.operations =
			static_cast<gphoto2::CameraOperation>(snapshot.operations);
		result.file_operations =
			static_cast<gphoto2::CameraFileOperation>(
				snapshot.fileOperations);
		result.folder_operations =
			static_cast<gphoto2::CameraFolderOperation>(
				snapshot.folderOperations);
		result.usb_vendor = snapshot.usbVendor;
		result.usb_product = snapshot.usbProduct;
		result.usb_class = snapshot.usbClass;
		result.usb_subclass = snapshot.usbSubclass;
		result.usb_protocol = snapshot.usbProtocol;
		std::memcpy(result.library, snapshot.library.c_str(),
			snapshot.library.size());
		std::memcpy(result.id, snapshot.id.c_str(), snapshot.id.size());
		result.device_type =
			static_cast<gphoto2::GphotoDeviceType>(snapshot.deviceType);
		return result;
	}

	static_assert(static_cast<unsigned>(CameraFileInfoField::Type) == gphoto2::GP_FILE_INFO_TYPE &&
		static_cast<unsigned>(CameraFileInfoField::Size) == gphoto2::GP_FILE_INFO_SIZE &&
		static_cast<unsigned>(CameraFileInfoField::Width) == gphoto2::GP_FILE_INFO_WIDTH &&
		static_cast<unsigned>(CameraFileInfoField::Height) == gphoto2::GP_FILE_INFO_HEIGHT &&
		static_cast<unsigned>(CameraFileInfoField::Permissions) == gphoto2::GP_FILE_INFO_PERMISSIONS &&
		static_cast<unsigned>(CameraFileInfoField::Status) == gphoto2::GP_FILE_INFO_STATUS &&
		static_cast<unsigned>(CameraFileInfoField::ModificationTime) == gphoto2::GP_FILE_INFO_MTIME &&
		static_cast<unsigned>(CameraFilePermission::Read) == gphoto2::GP_FILE_PERM_READ &&
		static_cast<unsigned>(CameraFilePermission::Delete) == gphoto2::GP_FILE_PERM_DELETE &&
		static_cast<int>(CameraFileStatus::NotDownloaded) == gphoto2::GP_FILE_STATUS_NOT_DOWNLOADED &&
		static_cast<int>(CameraFileStatus::Downloaded) == gphoto2::GP_FILE_STATUS_DOWNLOADED,
		"File information constants must match libgphoto2");

	static_assert(static_cast<unsigned>(CameraStorageInfoField::Base) == gphoto2::GP_STORAGEINFO_BASE &&
		static_cast<unsigned>(CameraStorageInfoField::Label) == gphoto2::GP_STORAGEINFO_LABEL &&
		static_cast<unsigned>(CameraStorageInfoField::Description) == gphoto2::GP_STORAGEINFO_DESCRIPTION &&
		static_cast<unsigned>(CameraStorageInfoField::Access) == gphoto2::GP_STORAGEINFO_ACCESS &&
		static_cast<unsigned>(CameraStorageInfoField::StorageType) == gphoto2::GP_STORAGEINFO_STORAGETYPE &&
		static_cast<unsigned>(CameraStorageInfoField::FilesystemType) == gphoto2::GP_STORAGEINFO_FILESYSTEMTYPE &&
		static_cast<unsigned>(CameraStorageInfoField::MaxCapacity) == gphoto2::GP_STORAGEINFO_MAXCAPACITY &&
		static_cast<unsigned>(CameraStorageInfoField::FreeSpaceKBytes) == gphoto2::GP_STORAGEINFO_FREESPACEKBYTES &&
		static_cast<unsigned>(CameraStorageInfoField::FreeSpaceImages) == gphoto2::GP_STORAGEINFO_FREESPACEIMAGES,
		"Storage field flags must match libgphoto2");
	static_assert(static_cast<int>(CameraStorageType::Unknown) == gphoto2::GP_STORAGEINFO_ST_UNKNOWN &&
		static_cast<int>(CameraStorageType::FixedRom) == gphoto2::GP_STORAGEINFO_ST_FIXED_ROM &&
		static_cast<int>(CameraStorageType::RemovableRom) == gphoto2::GP_STORAGEINFO_ST_REMOVABLE_ROM &&
		static_cast<int>(CameraStorageType::FixedRam) == gphoto2::GP_STORAGEINFO_ST_FIXED_RAM &&
		static_cast<int>(CameraStorageType::RemovableRam) == gphoto2::GP_STORAGEINFO_ST_REMOVABLE_RAM &&
		static_cast<int>(CameraStorageFilesystemType::Undefined) == gphoto2::GP_STORAGEINFO_FST_UNDEFINED &&
		static_cast<int>(CameraStorageFilesystemType::GenericFlat) == gphoto2::GP_STORAGEINFO_FST_GENERICFLAT &&
		static_cast<int>(CameraStorageFilesystemType::GenericHierarchical) == gphoto2::GP_STORAGEINFO_FST_GENERICHIERARCHICAL &&
		static_cast<int>(CameraStorageFilesystemType::Dcf) == gphoto2::GP_STORAGEINFO_FST_DCF &&
		static_cast<int>(CameraStorageAccessType::ReadWrite) == gphoto2::GP_STORAGEINFO_AC_READWRITE &&
		static_cast<int>(CameraStorageAccessType::ReadOnly) == gphoto2::GP_STORAGEINFO_AC_READONLY &&
		static_cast<int>(CameraStorageAccessType::ReadOnlyWithDelete) == gphoto2::GP_STORAGEINFO_AC_READONLY_WITH_DELETE,
		"Storage enum values must match libgphoto2");

	CameraWrapper::CameraWrapper(std::string const & model,
		std::string const & port)
		: CameraWrapper(model, port, CameraTimeoutCallbacks{})
	{
	}

	CameraWrapper::CameraWrapper(std::string const & model,
		std::string const & port,
		CameraTimeoutCallbacks const & timeoutCallbacks)
		: m_camera{nullptr}
		, m_context{}
		, m_timeoutState{makeTimeoutCallbackState(timeoutCallbacks)}
		, m_model{model}
		, m_port{port}
		, m_listenForEvents{false}
	{
		FILE_LOG(logINFO) << "CameraWrapper Constructor - model[" << m_model.c_str() << "], port[" << m_port.c_str() << "]";

		try
		{
			gphoto2pp::checkResponse(
				gphoto2::gp_camera_new(&m_camera), "gp_camera_new");
			initialize(m_model, m_port);
		}
		catch (...)
		{
			if (m_camera != nullptr)
			{
				if (m_cameraInitialized || m_timeoutState)
				{
					gphoto2pp::checkResponseSilent(
						gphoto2::gp_camera_exit(
							m_camera, m_context.getPtr()),
						"gp_camera_exit");
				}
				clearTimeoutCallbacks();
				gphoto2pp::checkResponseSilent(
					gphoto2::gp_camera_unref(m_camera),
					"gp_camera_unref");
				m_camera = nullptr;
			}
			throw;
		}
	}

	CameraWrapper::CameraWrapper()
		: CameraWrapper(CameraTimeoutCallbacks{})
	{
	}

	CameraWrapper::CameraWrapper(
		CameraTimeoutCallbacks const & timeoutCallbacks)
		: m_camera{nullptr}
		, m_context{}
		, m_timeoutState{makeTimeoutCallbackState(timeoutCallbacks)}
		, m_model{}
		, m_port{}
		, m_listenForEvents{false}
	{
		FILE_LOG(logINFO) << "CameraWrapper Constructor";

		try
		{
			gphoto2pp::checkResponse(
				gphoto2::gp_camera_new(&m_camera), "gp_camera_new");
			initialize();
		}
		catch (...)
		{
			if (m_camera != nullptr)
			{
				if (m_cameraInitialized || m_timeoutState)
				{
					gphoto2pp::checkResponseSilent(
						gphoto2::gp_camera_exit(
							m_camera, m_context.getPtr()),
						"gp_camera_exit");
				}
				clearTimeoutCallbacks();
				gphoto2pp::checkResponseSilent(
					gphoto2::gp_camera_unref(m_camera),
					"gp_camera_unref");
				m_camera = nullptr;
			}
			throw;
		}
	}

	CameraWrapper::~CameraWrapper()
	{
		FILE_LOG(logINFO) << "~CameraWrapper Destructor";

		stopListeningForEvents();

		if(m_camera != nullptr)
		{
			FILE_LOG(logINFO) << "CameraWrapper exit";
			if (m_cameraInitialized)
			{
				gphoto2pp::checkResponseSilent(
					gphoto2::gp_camera_exit(m_camera, m_context.getPtr()),
					"gp_camera_exit");
			}
			clearTimeoutCallbacks();
			gphoto2pp::checkResponse(gphoto2::gp_camera_unref(m_camera),"gp_camera_unref");
			m_camera = nullptr;
			m_cameraInitialized = false;
		}
	}

	ContextWrapper &CameraWrapper::getContext()
	{
		return m_context;
	}

	ContextWrapper const &CameraWrapper::getContext() const
	{
		return m_context;
	}

	void CameraWrapper::setTimeoutCallbacks(
		CameraTimeoutCallbacks const & timeoutCallbacks)
	{
		validateTimeoutCallbacks(timeoutCallbacks);
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}

		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		if (m_timeoutState &&
			m_timeoutState->hasActiveTimers())
		{
			throw exceptions::CameraWrapperException(
				"Cannot replace timeout callbacks while timers are active");
		}

		if (!timeoutCallbacks.start)
		{
			if (m_timeoutState)
			{
				m_timeoutState->deactivateIfIdle();
			}
			if (m_cameraInitialized)
			{
				gphoto2::gp_camera_set_timeout_funcs(
					m_camera, nullptr, nullptr, nullptr);
			}
			if (m_timeoutState)
			{
				m_timeoutState.reset();
			}
			return;
		}

		if (!m_timeoutState)
		{
			m_timeoutState =
				makeTimeoutCallbackState(timeoutCallbacks);
		}
		else
		{
			m_timeoutState->replaceCallbacks(timeoutCallbacks);
		}
		if (m_cameraInitialized)
		{
			installTimeoutCallbacks();
		}
	}
	
	CameraWrapper::CameraWrapper(CameraWrapper&& other)
		: m_camera{other.m_camera}
		, m_context{std::move(other.m_context)}
		, m_timeoutState{std::move(other.m_timeoutState)}
		, m_cameraInitialized{other.m_cameraInitialized}
		, m_model{std::move(other.m_model)}
		, m_port{std::move(other.m_port)}
		, m_listenForEvents{other.m_listenForEvents.load()}
	{
		FILE_LOG(logINFO) << "CameraWrapper move Constructor";
		
		m_listenForEvents = other.m_listenForEvents.load();
		
		// We cannot transfer the thread atomics over, so we have to stop listening in the previous class and start in the next one.
		other.stopListeningForEvents();
		
		m_cameraEvents = std::move(other.m_cameraEvents);
		
		// If the other CameraWrapper was listening to events, then we start listening to events here.
		if(m_listenForEvents)
		{
			startListeningForEvents();
		}
		
		other.m_camera = nullptr;
		other.m_cameraInitialized = false;
	}
	
	CameraWrapper& CameraWrapper::operator=(CameraWrapper&& other)
	{
		FILE_LOG(logINFO) << "CameraWrapper move assignment operator";
		
		if(this != &other)
		{
			// Stops current objects listening for events (it might not be, but better safe than sorry)
			stopListeningForEvents();
			
			// Release current objects resource
			if(m_camera != nullptr)
			{
				if (m_cameraInitialized)
				{
					gphoto2pp::checkResponseSilent(
						gphoto2::gp_camera_exit(m_camera, m_context.getPtr()),
						"gp_camera_exit");
				}
				clearTimeoutCallbacks();
				gphoto2pp::checkResponse(gphoto2::gp_camera_unref(m_camera),"gp_camera_unref");
			}
			
			// Steal or "move" the other objects resource
			m_camera = other.m_camera;
			m_context = std::move(other.m_context);
			m_timeoutState = std::move(other.m_timeoutState);
			m_cameraInitialized = other.m_cameraInitialized;
			m_model = std::move(other.m_model);
			m_port = std::move(other.m_port);
			
			m_listenForEvents = other.m_listenForEvents.load();
			
			// We cannot transfer the thread atomics over, so we have to stop listening in the previous class and start in the next one.
			other.stopListeningForEvents();
			
			m_cameraEvents = std::move(other.m_cameraEvents);
			
			// If the other CameraWrapper was listening to events, then we start listening to events here.
			if(m_listenForEvents)
			{
				startListeningForEvents();
			}
			
			// Unreference the other objects resource, so it's destructor doesn't unreference it
			other.m_camera = nullptr;
			other.m_cameraInitialized = false;
		}
		return *this;
	}
	
	std::string CameraWrapper::getModel() const
	{
		return m_model;
	}
		
	std::string CameraWrapper::getPort() const
	{
		return m_port;
	}

	PortInfoSnapshot CameraWrapper::getPortInfo() const
	{
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2::GPPortInfo portInfo = nullptr;
		gphoto2pp::checkResponse(
			gphoto2::gp_camera_get_port_info(m_camera, &portInfo),
			"gp_camera_get_port_info");
		if (portInfo == nullptr)
		{
			throw std::runtime_error(
				"Camera returned a null port information entry");
		}
		char *name = nullptr;
		char *path = nullptr;
		gphoto2::GPPortType type = gphoto2::GP_PORT_NONE;
		gphoto2pp::checkResponse(
			gphoto2::gp_port_info_get_name(portInfo, &name),
			"gp_port_info_get_name");
		gphoto2pp::checkResponse(
			gphoto2::gp_port_info_get_path(portInfo, &path),
			"gp_port_info_get_path");
		gphoto2pp::checkResponse(
			gphoto2::gp_port_info_get_type(portInfo, &type),
			"gp_port_info_get_type");
		if (name == nullptr || path == nullptr)
		{
			throw std::runtime_error(
				"Camera returned incomplete port information");
		}
		PortInfoSnapshot snapshot;
		snapshot.name = name;
		snapshot.path = path;
		snapshot.type = static_cast<GPPortTypeWrapper>(type);
		return snapshot;
	}

	void CameraWrapper::setPortInfo(PortInfoSnapshot const & info)
	{
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}
		if (info.name.find('\0') != std::string::npos ||
			info.path.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Port information strings must not contain embedded NUL");
		}
		gphoto2::GPPortInfo portInfo = nullptr;
		gphoto2::GPPortInfoList *list = nullptr;
		try
		{
			gphoto2pp::checkResponse(
				gphoto2::gp_port_info_list_new(&list),
				"gp_port_info_list_new");
			gphoto2pp::checkResponse(
				gphoto2::gp_port_info_new(&portInfo),
				"gp_port_info_new");
			gphoto2pp::checkResponse(
				gphoto2::gp_port_info_set_name(portInfo, info.name.c_str()),
				"gp_port_info_set_name");
			gphoto2pp::checkResponse(
				gphoto2::gp_port_info_set_path(portInfo, info.path.c_str()),
				"gp_port_info_set_path");
			gphoto2pp::checkResponse(
				gphoto2::gp_port_info_set_type(
					portInfo, static_cast<gphoto2::GPPortType>(info.type)),
				"gp_port_info_set_type");
			gphoto2pp::checkResponse(
				gphoto2::gp_port_info_list_append(list, portInfo),
				"gp_port_info_list_append");
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(
				gphoto2::gp_camera_set_port_info(m_camera, portInfo),
				"gp_camera_set_port_info");
		}
		catch (...)
		{
			if (list != nullptr)
			{
				gphoto2::gp_port_info_list_free(list);
			}
			throw;
		}
		gphoto2pp::checkResponseSilent(
			gphoto2::gp_port_info_list_free(list),
			"gp_port_info_list_free");
	}

	int CameraWrapper::getPortSpeed() const
	{
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		const int speed = gphoto2::gp_camera_get_port_speed(m_camera);
		gphoto2pp::checkResponse(speed, "gp_camera_get_port_speed");
		return speed;
	}

	void CameraWrapper::setPortSpeed(int speed)
	{
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(
			gphoto2::gp_camera_set_port_speed(m_camera, speed),
			"gp_camera_set_port_speed");
	}

	CameraAbilitiesSnapshot CameraWrapper::getAbilities() const
	{
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2::CameraAbilities source{};
		gphoto2pp::checkResponse(
			gphoto2::gp_camera_get_abilities(m_camera, &source),
			"gp_camera_get_abilities");
		return cameraAbilitiesToSnapshot(source);
	}

	void CameraWrapper::setAbilities(
		CameraAbilitiesSnapshot const & abilities)
	{
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}
		const gphoto2::CameraAbilities source =
			snapshotToCameraAbilities(abilities);
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(
			gphoto2::gp_camera_set_abilities(m_camera, source),
			"gp_camera_set_abilities");
	}

	void CameraWrapper::exit()
	{
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}
		if (!m_cameraInitialized)
		{
			throw exceptions::CameraWrapperException(
				"Camera is not initialized");
		}
		stopListeningForEvents();
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(
			gphoto2::gp_camera_exit(m_camera, m_context.getPtr()),
			"gp_camera_exit");
		clearTimeoutCallbacks();
		m_cameraInitialized = false;
	}

	void CameraWrapper::reinitialize()
	{
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}
		if (m_cameraInitialized)
		{
			throw exceptions::CameraWrapperException(
				"Camera is already initialized");
		}
		stopListeningForEvents();
		this->initialize();
	}

	void CameraWrapper::initialize()
	{
		// Connects to first camera found
		installTimeoutCallbacks();
		gphoto2pp::checkResponse(gphoto2::gp_camera_init(m_camera, m_context.getPtr()),"gp_camera_init");
		m_cameraInitialized = true;
		
		// Now that the camera is connected to, this will populate our m_model and m_port with the connected camera details
		gphoto2::GPPortInfo portInfo;
		gphoto2pp::checkResponse(gphoto2::gp_camera_get_port_info(m_camera, &portInfo),"gp_camera_get_port_info");
		
		const auto cameraAbilities = this->getAbilities();

		// Now we get the model and port and assign to our local variables
		m_model = cameraAbilities.model;
		
#if GPHOTO_LESS_25
		m_port = portInfo.path;
#else
		char * temp = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_port_info_get_path(portInfo, &temp),"gp_port_info_get_path");
		m_port = std::string(temp);
#endif
	}

	void CameraWrapper::initialize(std::string const & model, std::string const & port)
	{
		FILE_LOG(logINFO) << "Initializing Camera on Port: " << port << " with Model: " << model;
		
		CameraAbilitiesListWrapper cameraAbilitiesList;
		int model_index = cameraAbilitiesList.lookupModel(model);
		
		GPPortInfoListWrapper portInfoList;
		int port_index = portInfoList.lookupPath(port);
		
		const auto abilities =
			cameraAbilitiesList.getAbilities(model_index);
		// Sets camera abilities, probably should make this a member method
		this->setAbilities(abilities);
		
		gphoto2::GPPortInfo portInfo;
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_get_info(portInfoList.getPtr(), port_index, &portInfo),"gp_port_info_list_get_info");
		
		// Sets camera port info, probably should make this a member method
		gphoto2pp::checkResponse(gphoto2::gp_camera_set_port_info(m_camera, portInfo),"gp_camera_set_port_info");
		
		// Finally, init the camera
		installTimeoutCallbacks();
		gphoto2pp::checkResponse(gphoto2::gp_camera_init(m_camera, m_context.getPtr()),"gp_camera_init");
		m_cameraInitialized = true;
	}

	void CameraWrapper::installTimeoutCallbacks()
	{
		if (!m_timeoutState)
		{
			return;
		}
		m_timeoutState->activate(m_camera, m_context.getPtr());
		gphoto2::gp_camera_set_timeout_funcs(
			m_camera, cameraTimeoutStart, cameraTimeoutStop,
			m_timeoutState.get());
	}

	void CameraWrapper::clearTimeoutCallbacks()
	{
		if (m_camera != nullptr && m_timeoutState)
		{
			m_timeoutState->deactivate();
			gphoto2::gp_camera_set_timeout_funcs(
				m_camera, nullptr, nullptr, nullptr);
		}
	}

	std::string CameraWrapper::getSummary() const
	{
		gphoto2::CameraText text{};
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_get_summary(m_camera, &text, m_context.getPtr()),"gp_camera_get_summary");
		}
		
		return cameraTextToString(text);
	}

	std::string CameraWrapper::getManual() const
	{
		gphoto2::CameraText text{};

		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(
				gphoto2::gp_camera_get_manual(m_camera, &text, m_context.getPtr()),
				"gp_camera_get_manual");
		}

		return cameraTextToString(text);
	}

	std::string CameraWrapper::getAbout() const
	{
		gphoto2::CameraText text{};

		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(
				gphoto2::gp_camera_get_about(m_camera, &text, m_context.getPtr()),
				"gp_camera_get_about");
		}

		return cameraTextToString(text);
	}

	CameraFileWrapper CameraWrapper::capturePreview()
	{
		CameraFileWrapper cameraFile;
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_capture_preview(m_camera, cameraFile.getPtr(), m_context.getPtr()),"gp_camera_capture_preview");
		}
		
		return cameraFile;
	}

	CameraFilePathWrapper CameraWrapper::capture(CameraCaptureTypeWrapper const & captureType)
	{
		gphoto2::CameraFilePath cameraFilePath; // No wrapper made for this struct because it doesn't have an api to manipulate (similar to getSummary CameraText)
		
		// TODO, canon capture needs to enable the toggle widget "capture" with a value of 1, or 0 for off. I will need too get my hands on a canon and implement this.
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_capture(m_camera, static_cast<gphoto2::CameraCaptureType>(captureType),  &cameraFilePath, m_context.getPtr()),"gp_camera_capture");
		}
		
		FILE_LOG(logINFO) << "Pathname on the camera: '" << cameraFilePath.folder << "/" << cameraFilePath.name << "'";
		
		return gphoto2pp::CameraFilePathWrapper{cameraFilePath.name, cameraFilePath.folder};
	}
	
	void CameraWrapper::triggerCapture()
	{
#ifdef GPHOTO_LESS_25
		throw exceptions::InvalidLinkedVersionException("You are using a version of gphoto2 that doesn't support this command. Please link to gphoto 2.5 or greater");
#else	
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(gphoto2::gp_camera_trigger_capture(m_camera, m_context.getPtr()),"gp_camera_trigger_capture");
#endif
	}


	WindowWidget CameraWrapper::getConfig() const
	{
		gphoto2::CameraWidget* cameraWidget = nullptr;
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_get_config(m_camera, &cameraWidget, m_context.getPtr()),"gp_camera_get_config");
		}
		
		auto rootWidget = WindowWidget{cameraWidget};
		rootWidget.unref(); // This will have an extra ref added because of the constructor. And so we need to remove it.
		// std::move(rootWidget);
		return rootWidget;
	}

	std::unique_ptr<CameraWidgetWrapper> CameraWrapper::wrapConfigWidget(
		gphoto2::_CameraWidget *raw) const
	{
		gphoto2::CameraWidgetType type;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_type(raw, &type),
			"gp_widget_get_type");

		switch (type)
		{
			case gphoto2::GP_WIDGET_WINDOW:
				return std::unique_ptr<CameraWidgetWrapper>(new WindowWidget(raw));
			case gphoto2::GP_WIDGET_SECTION:
				return std::unique_ptr<CameraWidgetWrapper>(new NonValueWidget(raw));
			case gphoto2::GP_WIDGET_TEXT:
				return std::unique_ptr<CameraWidgetWrapper>(new TextWidget(raw));
			case gphoto2::GP_WIDGET_RANGE:
				return std::unique_ptr<CameraWidgetWrapper>(new RangeWidget(raw));
			case gphoto2::GP_WIDGET_TOGGLE:
				return std::unique_ptr<CameraWidgetWrapper>(new ToggleWidget(raw));
			case gphoto2::GP_WIDGET_RADIO:
				return std::unique_ptr<CameraWidgetWrapper>(new RadioWidget(raw));
			case gphoto2::GP_WIDGET_MENU:
				return std::unique_ptr<CameraWidgetWrapper>(new MenuWidget(raw));
			case gphoto2::GP_WIDGET_BUTTON:
				return std::unique_ptr<CameraWidgetWrapper>(new ButtonWidget(raw));
			case gphoto2::GP_WIDGET_DATE:
				return std::unique_ptr<CameraWidgetWrapper>(new DateWidget(raw));
			default:
				return std::unique_ptr<CameraWidgetWrapper>(
					new CameraWidgetWrapper(raw));
		}
	}

	std::vector<std::string> CameraWrapper::listConfig() const
	{
#ifndef GPHOTO2PP_HAVE_CAMERA_LIST_CONFIG
		throw exceptions::InvalidLinkedVersionException("Listing configuration requires gp_camera_list_config (libgphoto2 2.5.10 or greater)");
#else
		CameraListWrapper list;
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(
				gphoto2::gp_camera_list_config(m_camera, list.getPtr(), m_context.getPtr()),
				"gp_camera_list_config");
		}

		std::vector<std::string> names;
		const int count = list.count();
		names.reserve(static_cast<std::size_t>(count));
		for (int i = 0; i < count; ++i)
		{
			names.push_back(list.getName(i));
		}
		return names;
#endif
	}

	std::unique_ptr<CameraWidgetWrapper> CameraWrapper::getSingleConfig(
		std::string const & name) const
	{
#ifndef GPHOTO2PP_HAVE_CAMERA_GET_SINGLE_CONFIG
		(void)name;
		throw exceptions::InvalidLinkedVersionException("Reading a single configuration requires gp_camera_get_single_config (libgphoto2 2.5.10 or greater)");
#else
		if (name.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException("Configuration name contains an embedded NUL");
		}
		gphoto2::_CameraWidget *raw = nullptr;
		int result;
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			result = gphoto2::gp_camera_get_single_config(
				m_camera, name.c_str(), &raw, m_context.getPtr());
		}
		std::unique_ptr<gphoto2::_CameraWidget, decltype(&gphoto2::gp_widget_unref)>
			owned(raw, &gphoto2::gp_widget_unref);
		gphoto2pp::checkResponse(result, "gp_camera_get_single_config");
		if (raw == nullptr)
		{
			throw exceptions::NullWidget("Camera returned a null configuration widget");
		}
		auto widget = this->wrapConfigWidget(raw);
		return widget;
#endif
	}

	void CameraWrapper::setSingleConfig(std::string const & name,
		CameraWidgetWrapper const & widget) const
	{
#if !defined(GPHOTO2PP_HAVE_CAMERA_GET_SINGLE_CONFIG) || \
	!defined(GPHOTO2PP_HAVE_CAMERA_SET_SINGLE_CONFIG)
		(void)name;
		(void)widget;
		throw exceptions::InvalidLinkedVersionException("Setting a single configuration requires gp_camera_get_single_config and gp_camera_set_single_config (libgphoto2 2.5.10 or greater)");
#else
		if (name.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Configuration name contains an embedded NUL");
		}
		if (widget.m_cameraWidget == nullptr)
		{
			throw exceptions::NullWidget("Configuration widget is empty");
		}
		if (widget.getName() != name)
		{
			throw exceptions::ArgumentException(
				"Configuration widget name does not match the requested name");
		}

		const auto type = widget.getType();
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2::_CameraWidget *raw = nullptr;
		const int result = gphoto2::gp_camera_get_single_config(
			m_camera, name.c_str(), &raw, m_context.getPtr());
		std::unique_ptr<gphoto2::_CameraWidget,
			decltype(&gphoto2::gp_widget_unref)> owned(raw, &gphoto2::gp_widget_unref);
		gphoto2pp::checkResponse(result, "gp_camera_get_single_config");
		if (raw == nullptr)
		{
			throw exceptions::NullWidget(
				"Camera returned a null configuration widget");
		}
		auto cameraWidget = this->wrapConfigWidget(raw);
		if (cameraWidget->getType() != type)
		{
			throw exceptions::InvalidWidgetType(
				"Configuration widget type does not match the camera");
		}

		copyConfigValue(*cameraWidget, widget, type);
		gphoto2pp::checkResponse(gphoto2::gp_camera_set_single_config(
			m_camera, name.c_str(), cameraWidget->m_cameraWidget, m_context.getPtr()),
			"gp_camera_set_single_config");
#endif
	}

	void CameraWrapper::pressButton(std::string const & name) const
	{
		if (name.empty() || name.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Button name must be nonempty and contain no embedded NUL");
		}
		if (m_camera == nullptr)
		{
			throw exceptions::CameraWrapperException(
				"Camera wrapper is not initialized");
		}

		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2::_CameraWidget *rawRoot = nullptr;
		const int result = gphoto2::gp_camera_get_config(
			m_camera, &rawRoot, m_context.getPtr());
		std::unique_ptr<gphoto2::_CameraWidget,
			decltype(&gphoto2::gp_widget_unref)> root(
				rawRoot, &gphoto2::gp_widget_unref);
		gphoto2pp::checkResponse(result, "gp_camera_get_config");
		if (rawRoot == nullptr)
		{
			throw exceptions::NullWidget(
				"Camera returned a null configuration widget");
		}

		std::vector<gphoto2::_CameraWidget *> matches;
		findWidgetsByName(rawRoot, name, matches);
		if (matches.empty())
		{
			throw exceptions::ArgumentException(
				"No configuration widget has the requested name");
		}
		if (matches.size() != 1)
		{
			throw exceptions::ArgumentException(
				"Configuration widget name is ambiguous");
		}

		auto widget = this->wrapConfigWidget(matches.front());
		auto *button = dynamic_cast<ButtonWidget *>(widget.get());
		if (button == nullptr)
		{
			throw exceptions::InvalidWidgetType(
				"Named configuration widget is not a button");
		}

		gphoto2::CameraWidgetCallback callback = nullptr;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_value(
			button->getPtr(), &callback), "gp_widget_get_value");
		if (callback == nullptr)
		{
			throw exceptions::NullWidget("Camera button has no callback");
		}
		gphoto2pp::checkResponse(
			callback(m_camera, button->getPtr(), m_context.getPtr()),
			"CameraWidgetCallback");
	}

	void CameraWrapper::setConfig(CameraWidgetWrapper const & cameraWidget)
	{
		auto rootWidget = cameraWidget.getRoot();
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_set_config(m_camera, rootWidget.getPtr(), m_context.getPtr()),"gp_camera_set_config"); // we can use cameraWidget->m_cameraWidget because this is a friend class of the camera_widget_wrapper
		}
	}

	std::vector<CameraStorageInfo> CameraWrapper::getStorageInfo() const
	{
		gphoto2::CameraStorageInformation *raw = nullptr;
		int count = 0;
		int result;
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			result = gphoto2::gp_camera_get_storageinfo(
				m_camera, &raw, &count, m_context.getPtr());
		}
		std::unique_ptr<gphoto2::CameraStorageInformation, decltype(&std::free)>
			storage(raw, &std::free);
		gphoto2pp::checkResponse(result, "gp_camera_get_storageinfo");
		if (count < 0 || (count > 0 && raw == nullptr))
		{
			throw std::runtime_error("Camera returned invalid storage information");
		}
		std::vector<CameraStorageInfo> entries;
		entries.reserve(static_cast<std::size_t>(count));
		for (int i = 0; i < count; ++i)
		{
			const auto &source = raw[i];
			CameraStorageInfo entry;
			entry.fields = static_cast<std::uint32_t>(source.fields);
			if (entry.has(CameraStorageInfoField::Base))
			{
				entry.baseDirectory = metadataString(source.basedir, "storage base directory");
			}
			if (entry.has(CameraStorageInfoField::Label))
			{
				entry.label = metadataString(source.label, "storage label");
			}
			if (entry.has(CameraStorageInfoField::Description))
			{
				entry.description = metadataString(source.description, "storage description");
			}
			if (entry.has(CameraStorageInfoField::StorageType))
			{
				entry.storageType = static_cast<CameraStorageType>(source.type);
			}
			if (entry.has(CameraStorageInfoField::FilesystemType))
			{
				entry.filesystemType = static_cast<CameraStorageFilesystemType>(source.fstype);
			}
			if (entry.has(CameraStorageInfoField::Access))
			{
				entry.access = static_cast<CameraStorageAccessType>(source.access);
			}
			if (entry.has(CameraStorageInfoField::MaxCapacity))
			{
				entry.capacityKBytes = source.capacitykbytes;
			}
			if (entry.has(CameraStorageInfoField::FreeSpaceKBytes))
			{
				entry.freeKBytes = source.freekbytes;
			}
			if (entry.has(CameraStorageInfoField::FreeSpaceImages))
			{
				entry.freeImages = source.freeimages;
			}
			entries.push_back(std::move(entry));
		}
		return entries;
	}

	bool CameraWrapper::startListeningForEvents()
	{
		std::lock_guard<std::mutex> lock{m_eventListenerMutex};
		if(m_listenForEventSignalFuture.valid() &&
			m_listenForEventSignalFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
		{
			FILE_LOG(logWARN1) << "Already listening to events";
			return true;
		}
		m_listenForEventSignalFuture = std::future<bool>{};
		
		// Set to true so our thread loop will repeat until set back
		m_listenForEvents = true;
					
		std::packaged_task<bool()> event_task([this](){
			FILE_LOG(logINFO) << "Starting to listen for camera events";
			
			while(m_listenForEvents == true)
			{
				gphoto2::CameraEventType eventType;
				void* eventData;
				
				try
				{
					std::lock_guard<std::mutex> lock{m_cameraIOMutex};
					gphoto2pp::checkResponse(gphoto2::gp_camera_wait_for_event(m_camera, 400, &eventType, &eventData, m_context.getPtr()),"gp_camera_wait_for_event");
				}
				catch (...)
				{
					m_listenForEvents = false;
					FILE_LOG(logCRITICAL) << "An Exception was thrown when waiting for camera events. The Camera possibly lost connection with the computer";					
					return false;
				}
				
				FILE_LOG(logINFO) << "EventType Received: '" << static_cast<int>(eventType) << "'";
				
				switch(eventType)
				{
					case gphoto2::GP_EVENT_UNKNOWN:
					{
						if(eventData)
						{
							// Unknown event, but it has data
							m_cameraEvents(static_cast<CameraEventTypeWrapper>(eventType), CameraFilePathWrapper{"",""}, std::string((char*)eventData));
							break; // Break out of case
						}
						// Unknown event without data, so we let it trickle through
					}
					case gphoto2::GP_EVENT_TIMEOUT:
					case gphoto2::GP_EVENT_CAPTURE_COMPLETE:
					{
						m_cameraEvents(static_cast<CameraEventTypeWrapper>(eventType), CameraFilePathWrapper{"",""}, std::string("No Event Data Returned"));
						break;
					}
					case gphoto2::GP_EVENT_FILE_ADDED:
					case gphoto2::GP_EVENT_FOLDER_ADDED:
					case gphoto2::GP_EVENT_FILE_CHANGED:
					{
						gphoto2::CameraFilePath* cameraFilePath = (gphoto2::CameraFilePath*)eventData;
						m_cameraEvents(static_cast<CameraEventTypeWrapper>(eventType), CameraFilePathWrapper{cameraFilePath->name, cameraFilePath->folder}, std::string(""));
						break;
					}
					default:
					{
						FILE_LOG(logWARN) << "Un-recognized EventType Fired: '" << eventType << "'";
						break;
					}
				}
			}
			FILE_LOG(logINFO) << "Stopped to listen for camera events";
			return true;
		});
		
		m_listenForEventSignalFuture = event_task.get_future();
		
		std::thread(std::move(event_task)).detach();

		return true;
	}
	
	observer::Registration CameraWrapper::subscribeToCameraEvent(CameraEventTypeWrapper const & event, std::function<void(const CameraFilePathWrapper&, const std::string&)> func)
	{
		return m_cameraEvents.registerObserver(event, std::move(func));
	}
	
	void CameraWrapper::stopListeningForEvents()
	{
		std::lock_guard<std::mutex> lock{m_eventListenerMutex};
		m_listenForEvents = false;
		if(m_listenForEventSignalFuture.valid())
		{
			FILE_LOG(logDEBUG) << "listener is running, we will signal it to stop";
			
			// TODO probably make this a wait_for with a timeout, so we cannot freeze anything...
			m_listenForEventSignalFuture.wait();
			m_listenForEventSignalFuture = std::future<bool>{};
			
			FILE_LOG(logDEBUG) << "listener just stopped";
		}
		else
		{
			FILE_LOG(logDEBUG) << "listener is already stopped";
		}
	}
	
	CameraListWrapper CameraWrapper::folderListFiles(std::string const & folder) const
	{
		CameraListWrapper cameraList;
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_folder_list_files(m_camera, folder.c_str(), cameraList.getPtr(), m_context.getPtr()),"gp_camera_folder_list_files");
		}
		
		return cameraList;
	}
	
	CameraListWrapper CameraWrapper::folderListFolders(std::string const & folder) const
	{
		CameraListWrapper cameraList;
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_folder_list_folders(m_camera, folder.c_str(), cameraList.getPtr(), m_context.getPtr()),"gp_camera_folder_list_folders");
		}
		
		return cameraList;
	}
	
	void CameraWrapper::folderDeleteAll(std::string const & folder)
	{
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_delete_all(m_camera, folder.c_str(), m_context.getPtr()),"gp_camera_folder_delete_all");
	}
	
	void CameraWrapper::folderPutFile(std::string const & folder, std::string const & fileName, CameraFileTypeWrapper const & fileType, CameraFileWrapper cameraFile)
	{
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
#ifdef GPHOTO_LESS_25
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_put_file(m_camera, folder.c_str(), cameraFile.getPtr(), m_context.getPtr()),"gp_camera_folder_put_file");
#else
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_put_file(m_camera, folder.c_str(), fileName.c_str(), static_cast<gphoto2::CameraFileType>(fileType), cameraFile.getPtr(), m_context.getPtr()),"gp_camera_folder_put_file");
#endif
	}
	
	void CameraWrapper::folderMakeDir(std::string const & folder, std::string const & name)
	{
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_make_dir(m_camera, folder.c_str(), name.c_str(), m_context.getPtr()),"gp_camera_folder_make_dir");
	}
	
	void CameraWrapper::folderRemoveDir(std::string const & folder, std::string const & name)
	{
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_remove_dir(m_camera, folder.c_str(), name.c_str(), m_context.getPtr()),"gp_camera_folder_remove_dir");
	}
	
	CameraFileWrapper CameraWrapper::fileGet(std::string const & folder, std::string const & fileName, CameraFileTypeWrapper const & fileType) const
	{
		CameraFileWrapper cameraFileWrapper;
		
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_file_get(m_camera, folder.c_str(), fileName.c_str(), static_cast<gphoto2::CameraFileType>(fileType), cameraFileWrapper.getPtr(), m_context.getPtr()),"gp_camera_file_get");
		}
		
		return cameraFileWrapper;
	}

	std::vector<char> CameraWrapper::fileRead(std::string const & folder,
		std::string const & fileName, CameraFileTypeWrapper const & fileType,
		std::uint64_t offset, std::uint64_t size) const
	{
		if (folder.find('\0') != std::string::npos ||
			fileName.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera file path contains an embedded NUL");
		}

		if (size == 0)
		{
			return {};
		}

		std::vector<char> data;
		if (size > data.max_size())
		{
			throw exceptions::ArgumentException(
				"Camera file read size exceeds vector capacity");
		}
		data.resize(static_cast<std::size_t>(size));

		std::uint64_t bytesRead = size;
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_file_read(
				m_camera, folder.c_str(), fileName.c_str(),
				static_cast<gphoto2::CameraFileType>(fileType), offset,
				data.data(), &bytesRead, m_context.getPtr()),
				"gp_camera_file_read");
		}

		if (bytesRead > size)
		{
			throw std::runtime_error(
				"Camera returned more file data than requested");
		}
		data.resize(static_cast<std::size_t>(bytesRead));
		return data;
	}
	
	CameraFileInfo CameraWrapper::fileGetInfo(
		std::string const & folder, std::string const & fileName) const
	{
		if (folder.find('\0') != std::string::npos ||
			fileName.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera file path contains an embedded NUL");
		}

		gphoto2::CameraFileInfo raw{};
		{
			std::lock_guard<std::mutex> lock{m_cameraIOMutex};
			gphoto2pp::checkResponse(gphoto2::gp_camera_file_get_info(
				m_camera, folder.c_str(), fileName.c_str(), &raw, m_context.getPtr()),
				"gp_camera_file_get_info");
		}

		CameraFileInfo info;
		info.file.common = fileInfoPart(raw.file);
		info.preview.common = fileInfoPart(raw.preview);
		info.audio = fileInfoPart(raw.audio);
		if (info.file.common.has(CameraFileInfoField::Width))
		{
			info.file.width = raw.file.width;
		}
		if (info.file.common.has(CameraFileInfoField::Height))
		{
			info.file.height = raw.file.height;
		}
		if (info.file.common.has(CameraFileInfoField::Permissions))
		{
			info.file.permissions = static_cast<std::uint32_t>(raw.file.permissions);
		}
		if (info.file.common.has(CameraFileInfoField::ModificationTime))
		{
			info.file.modificationTime = raw.file.mtime;
		}
		if (info.preview.common.has(CameraFileInfoField::Width))
		{
			info.preview.width = raw.preview.width;
		}
		if (info.preview.common.has(CameraFileInfoField::Height))
		{
			info.preview.height = raw.preview.height;
		}
		return info;
	}

	void CameraWrapper::fileSetInfo(std::string const & folder,
		std::string const & fileName, CameraFileInfoUpdate const & update) const
	{
		if (folder.find('\0') != std::string::npos ||
			fileName.find('\0') != std::string::npos)
		{
			throw exceptions::ArgumentException(
				"Camera file path contains an embedded NUL");
		}

		const auto permitted = static_cast<std::uint32_t>(CameraFilePermission::Read) |
			static_cast<std::uint32_t>(CameraFilePermission::Delete);
		if (!update.updatePermissions && !update.updateModificationTime)
		{
			throw exceptions::ArgumentException("File metadata update has no selected fields");
		}
		if (update.updatePermissions && (update.permissions & ~permitted) != 0)
		{
			throw exceptions::ArgumentException("File metadata update has invalid permission bits");
		}

		gphoto2::CameraFileInfo raw{};
		unsigned fields = 0;
		if (update.updatePermissions)
		{
			fields |= gphoto2::GP_FILE_INFO_PERMISSIONS;
			raw.file.permissions = static_cast<gphoto2::CameraFilePermissions>(update.permissions);
		}
		if (update.updateModificationTime)
		{
			fields |= gphoto2::GP_FILE_INFO_MTIME;
			raw.file.mtime = update.modificationTime;
		}
		raw.file.fields = static_cast<gphoto2::CameraFileInfoFields>(fields);
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(gphoto2::gp_camera_file_set_info(
			m_camera, folder.c_str(), fileName.c_str(), raw, m_context.getPtr()),
			"gp_camera_file_set_info");
	}

	void CameraWrapper::fileDelete(std::string const & folder, std::string const & fileName) const
	{
		std::lock_guard<std::mutex> lock{m_cameraIOMutex};
		gphoto2pp::checkResponse(gphoto2::gp_camera_file_delete(m_camera, folder.c_str(), fileName.c_str(), m_context.getPtr()),"gp_camera_file_delete");
	}
}

