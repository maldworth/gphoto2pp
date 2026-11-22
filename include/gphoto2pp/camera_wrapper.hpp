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

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <gphoto2pp/observer.hpp>
#include <gphoto2pp/camera_file_info.hpp>
#include <gphoto2pp/camera_storage_info.hpp>
#include <gphoto2pp/camera_widget_wrapper.hpp>
#include <gphoto2pp/helper_context.hpp>
#include <gphoto2pp/camera_abilities_list_wrapper.hpp>

#include <string>
#include <iosfwd>
#include <cstdint>
#include <future>
#include <mutex>
#include <memory>
#include <vector>
#include <functional>

namespace gphoto2
{
	struct _Camera;
}

namespace gphoto2pp
{
	//Forward Declarations
	enum class CameraEventTypeWrapper : int;
	enum class CameraFileTypeWrapper : int;
	enum class CameraCaptureTypeWrapper : int;
	
	struct CameraFilePathWrapper;
	struct PortInfoSnapshot;
	
	class ButtonWidget;
	class CameraFileWrapper;
	class WindowWidget;
	class CameraListWrapper;
	
	using CameraTimeoutAction = std::function<int()>;

	/**
	 * \brief Host-provided scheduler for camera-driver keep-alive callbacks.
	 *
	 * The start callback receives a repeat interval in seconds and an action
	 * that returns a libgphoto2 result code. It must schedule the action
	 * asynchronously rather than invoke it inline. The returned timer ID must
	 * fit in a nonnegative int and be unique among active timers. The stop
	 * callback must synchronously prevent further action calls and wait for
	 * any in-flight call to finish.
	 */
	struct CameraTimeoutCallbacks
	{
		std::function<unsigned int(
			unsigned int, CameraTimeoutAction)> start;
		std::function<void(unsigned int)> stop;
	};

	namespace detail
	{
		struct CameraTimeoutCallbackState;
	}

	class CameraWrapper
	{
	public:
		/**
		 * @brief Initializes the first detected camera without a timeout scheduler.
		 */
		CameraWrapper();

		/**
		 * \brief Initializes the first detected camera with a host timeout scheduler.
		 * \param[in] timeoutCallbacks host callbacks used for driver keep-alive
		 * timers; both callbacks must be set or both empty
		 * \throw gphoto2pp::exceptions::ArgumentException if only one
		 * callback is set
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on camera failure
		 */
		explicit CameraWrapper(
			CameraTimeoutCallbacks const & timeoutCallbacks);
		
		/**
		 * @brief Connects and initializes using the provided model and port.
		 */
		CameraWrapper(std::string const & model, std::string const & port);

		/**
		 * \brief Initializes the selected camera with a host timeout scheduler.
		 * \param[in] model camera model
		 * \param[in] port camera port
		 * \param[in] timeoutCallbacks host callbacks used for driver keep-alive
		 * timers; both callbacks must be set or both empty
		 * \throw gphoto2pp::exceptions::ArgumentException if only one
		 * callback is set
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on camera failure
		 */
		CameraWrapper(std::string const & model, std::string const & port,
			CameraTimeoutCallbacks const & timeoutCallbacks);

		/**
		 * \brief Gets the context owner used by camera operations.
		 * \return Borrowed context wrapper; valid as long as this camera wrapper
		 * remains in the same object and has not been moved from
		 */
		ContextWrapper &getContext();
		ContextWrapper const &getContext() const;

		/**
		 * \brief Sets or replaces the host scheduler for driver keep-alive timers.
		 * \param[in] timeoutCallbacks both callbacks must be set or both empty
		 * \note Configure through the constructor when camera initialization
		 * itself may request a keep-alive timer. A host scheduler must not run
		 * the supplied action inline from start and its stop callback must
		 * synchronously cancel and drain it.
		 * \throw gphoto2pp::exceptions::ArgumentException if only one callback
		 * is set
		 * \throw GPhoto2pp::exceptions::CameraWrapperException if a timeout is
		 * active or the wrapper has been moved from
		 */
		void setTimeoutCallbacks(
			CameraTimeoutCallbacks const & timeoutCallbacks);

		~CameraWrapper();
		
		// Move constructor and move assignment are allowed.
		CameraWrapper(CameraWrapper&& other);
		CameraWrapper& operator=(CameraWrapper&& other);
		
		// This is not copyable. the t_Camera object is sort of a singleton. We don't want multiple commands being issued to one t_Camera resource.
		CameraWrapper(const CameraWrapper& other) = delete;
		CameraWrapper& operator=(CameraWrapper const & other) = delete;
		
		/**
		 * \brief Gets the complete summary of the currently connected camera.
		 * This summary indicates numerous details about make, model, and configurable abilities.
		 * \return the summary
		 * \note Direct wrapper for <tt>gp_camera_get_summary(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getSummary() const;

		/**
		 * \brief Gets the camera-provided user manual text.
		 * \return The manual text, which may be empty if the camera provides none.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * \note The result is copied from libgphoto2's fixed-size CameraText.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception if the camera reports
		 * an error, including unsupported functionality.
		 */
		std::string getManual() const;

		/**
		 * \brief Gets the camera-provided about text.
		 * \return The about text, which may be empty if the camera provides none.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * \note The result is copied from libgphoto2's fixed-size CameraText.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception if the camera reports
		 * an error, including unsupported functionality.
		 */
		std::string getAbout() const;
		
		/**
		 * \brief Gets the camera model currently connected to.
		 * \return the model name
		 */
		std::string getModel() const;
		
		/**
		 * \brief Gets the computer port currently connected to the camera.
		 * \return the port name
		 */
		std::string getPort() const;

		/**
		 * \brief Gets the port information assigned to the connected camera.
		 * \return Owned port name, path and type snapshot.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 * \throw std::runtime_error for incomplete port information
		 */
		PortInfoSnapshot getPortInfo() const;

		/**
		 * \brief Sets camera port information.
		 * \param[in] info port name, path and type
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setPortInfo(PortInfoSnapshot const & info);

		/**
		 * \brief Gets the current camera port speed.
		 * \return The speed reported by libgphoto2.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int getPortSpeed() const;

		/**
		 * \brief Requests a camera port speed.
		 * \param[in] speed The speed to pass to libgphoto2.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * Setting a speed can prevent a driver from selecting its optimal
		 * value; support depends on the camera and port.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setPortSpeed(int speed);

		/**
		 * \brief Gets the abilities assigned to the connected camera.
		 * \return Owned camera abilities snapshot.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw std::runtime_error for unterminated fixed strings
		 */
		CameraAbilitiesSnapshot getAbilities() const;

		/**
		 * \brief Sets camera abilities.
		 * \param[in] abilities snapshot to pass to libgphoto2
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL,
		 * oversized fixed strings, or more than 64 serial speeds
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 */
		void setAbilities(CameraAbilitiesSnapshot const & abilities);

		/**
		 * \brief Releases the initialized camera connection.
		 * \note Event listening is stopped before camera I/O. The camera object
		 * remains valid and may be initialized again with init-equivalent
		 * constructors or a future reinitialize API.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw GPhoto2pp::exceptions::CameraWrapperException if moved from
		 */
		void exit();

		/**
		 * \brief Reinitializes a camera after an explicit exit.
		 * \note Redetects and initializes the first usable camera. The wrapper
		 * refreshes model and port values after success.
		 * \throw GPhoto2pp::exceptions::CameraWrapperException if moved from
		 * or already initialized
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on initialization
		 */
		void reinitialize();
		
		/**
		 * \brief Captures a preview image from the camera.
		 * This capture type might not be supported by all cameras (requires a live view/mirror lockup mode for continuous captures). The image does not persist on the camera.
		 * \return the image captured
		 * \note Direct wrapper for <tt>gp_camera_capture_preview(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraFileWrapper capturePreview();
		
		/**
		 * \brief Captures a file from the camera.
		 * \param[in]	captureType	of file to retrieve from the camera
		 * \return the file path
		 * \note Direct wrapper for <tt>gp_camera_capture(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraFilePathWrapper capture(CameraCaptureTypeWrapper const & captureType);
		
		/**
		 * \brief Triggers the camera to take a picture (similar to a remote shutter release).
		 * The image is saved to the camera's portable storage for retrieval later
		 * \note Direct wrapper for <tt>gp_camera_trigger_capture(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void triggerCapture();
		
		/**
		 * \brief Queries all the properties/abilities on the camera and compiles them in a Widget N-way tree hierarchy.
		 * \return the Root widget (which will always be of type Window Widget)
		 * \note Direct wrapper for <tt>gp_camera_get_config(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		WindowWidget getConfig() const;

		/**
		 * \brief Lists configuration names for libgphoto2 single-setting calls.
		 * \return Owned names in the order reported by the camera, including
		 * duplicates; an empty list means no names were reported.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * libgphoto2 may obtain the full configuration as a fallback.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw GPhoto2pp::exceptions::InvalidLinkedVersionException when
		 * the selected libgphoto2 lacks this API (introduced in 2.5.10)
		 */
		std::vector<std::string> listConfig() const;

		/**
		 * \brief Retrieves one named configuration widget.
		 * \param[in] name configuration name, without embedded NUL bytes
		 * \return Owning polymorphic widget; use getType and dynamic_cast to
		 * access its type-specific interface. Unknown types have only the base
		 * interface. Button widgets can be invoked with @ref pressButton.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * libgphoto2 may retrieve the full configuration as a fallback.
		 * The returned widget is independent of the camera wrapper's lifetime.
		 * Changes are local until passed to @ref setSingleConfig.
		 * \throw GPhoto2pp::exceptions::ArgumentException for an embedded NUL
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw GPhoto2pp::exceptions::NullWidget for a null result
		 * \throw GPhoto2pp::exceptions::InvalidLinkedVersionException when
		 * the selected libgphoto2 lacks this API (introduced in 2.5.10)
		 */
		std::unique_ptr<CameraWidgetWrapper> getSingleConfig(
			std::string const & name) const;

		/**
		 * \brief Sets one named configuration value.
		 * \param[in] name configuration name, without embedded NUL bytes
		 * \param[in] widget widget containing the new value and the same name
		 * \note The camera's current widget is queried to verify the type and
		 * to preserve camera-provided widget metadata. Read and write are
		 * serialized as one camera operation.
		 * A driver may report an error after changing the value; re-read before
		 * retrying. Button widgets are actions and must be invoked with
		 * @ref pressButton instead.
		 * \throw GPhoto2pp::exceptions::ArgumentException for an embedded NUL
		 * or a widget whose name differs from name
		 * \throw GPhoto2pp::exceptions::InvalidWidgetType if the widget type
		 * is not a supported value type or differs from the camera widget
		 * \throw GPhoto2pp::exceptions::NullWidget if the source value is unset
		 * \throw GPhoto2pp::exceptions::ArgumentException if a date does not fit
		 * libgphoto2's integer representation
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw GPhoto2pp::exceptions::InvalidLinkedVersionException when
		 * the selected libgphoto2 lacks this API (introduced in 2.5.10)
		 */
		void setSingleConfig(std::string const & name,
			CameraWidgetWrapper const & widget) const;

		/**
		 * \brief Invokes a uniquely named button in the current camera config.
		 * \param[in] name nonempty widget name without embedded NUL bytes
		 * \note The current configuration is fetched for each invocation.
		 * The callback runs under the camera-I/O lock with the current camera,
		 * widget, and context. Duplicate names are rejected rather than
		 * invoking an arbitrary button.
		 * \warning Button callbacks may cause immediate camera-side changes.
		 * \throw GPhoto2pp::exceptions::ArgumentException for an empty,
		 * embedded-NUL, missing, or ambiguous name
		 * \throw GPhoto2pp::exceptions::InvalidWidgetType when the named
		 * widget is not a button
		 * \throw GPhoto2pp::exceptions::NullWidget when the config or button
		 * callback is null
		 * \throw GPhoto2pp::exceptions::CameraWrapperException if this wrapper
		 * has been moved from
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 or
		 * callback failure
		 */
		void pressButton(std::string const & name) const;
		
		/**
		 * \brief Sets the widgets to the provided settings. It's important to
		 * note, that if camera settings change (manually by physical
		 * interaction), and then you call this method with the old settings, it
		 * will change the camera back to the old settings. It's best practice
		 * to query the camera, change the settings, and then immediately set
		 * the config again.
         * \param[in]   cameraWidget    to traverse and write
		 * all settings to the camera
         * \return the Root widget (which will always
		 * be of type Window Widget)
         * \note Direct wrapper for
		 * <tt>gp_camera_set_config(...)</tt>
         * \throw
		 * GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setConfig(CameraWidgetWrapper const & cameraWidget);

		/**
		 * \brief Gets information about attached camera storage.
		 * \return Owned snapshots, empty if the camera reports no storage.
		 * \note Fields are valid only when their corresponding flag is set.
		 * Camera I/O is serialized with other CameraWrapper operations.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure,
		 * including unsupported operations
		 * \throw std::runtime_error for malformed storage information
		 */
		std::vector<CameraStorageInfo> getStorageInfo() const;
		
		//Filesystem Operations
		/**
		 * \brief Lists all files in the provided folder
		 * \param[in]	folder	to list all files in
		 * \return the list of files
		 * \note Direct wrapper for <tt>gp_camera_folder_list_files(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraListWrapper folderListFiles(std::string const & folder) const;
		
		/**
		 * \brief Lists all folders in the provided folder
		 * \param[in]	folder	to list all folders in
		 * \return the list of folders
		 * \note Direct wrapper for <tt>gp_camera_folder_list_folders(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraListWrapper folderListFolders(std::string const & folder) const;
		
		/**
		 * \brief Delete all files in the provided folder
		 * \param[in]	folder	to delete all files
		 * \note Direct wrapper for <tt>gp_camera_folder_delete_all(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void folderDeleteAll(std::string const & folder);
		
		/**
		 * \brief Write a file in the provided folder
		 * \param[in]	folder	to write the new file in
		 * \param[in]	fileName	for the new file to be written
		 * \param[in]	fileType	for the new file to be written
		 * \param[in]	cameraFile	contains the new file to be written to the
         * folder
		 * \note Direct wrapper for <tt>gp_camera_folder_put_file(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void folderPutFile(std::string const & folder, std::string const & fileName, CameraFileTypeWrapper const & fileType, CameraFileWrapper cameraFile);
		
		/**
		 * \brief Make a new folder in the provided directory
		 * \param[in]	folder	to make the new folder in
		 * \param[in]	name	for the new folder
		 * \note Direct wrapper for <tt>gp_camera_folder_make_dir(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void folderMakeDir(std::string const & folder, std::string const & name);
		
		/**
		 * \brief Remove a directory from the provided folder
		 * \param[in]	folder	containing the folder to remove
		 * \param[in]	name	of the folder to remove
		 * \note Direct wrapper for <tt>gp_camera_folder_remove_dir(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void folderRemoveDir(std::string const & folder, std::string const & name);
		
		/**
		 * \brief Retrieve a file from the camera
		 * \param[in]	folder	containing the file to get
		 * \param[in]	fileName	of the file to retrieve
		 * \param[in]	fileType	of the file to retrieve
		 * \return the file
		 * \note Direct wrapper for <tt>gp_camera_file_get(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraFileWrapper fileGet(std::string const & folder, std::string const & fileName, CameraFileTypeWrapper const & fileType) const;

		/**
		 * \brief Reads a bounded chunk from a camera file.
		 * \param[in] folder folder containing the file
		 * \param[in] fileName file name within the folder
		 * \param[in] fileType part of the file to read
		 * \param[in] offset byte offset at which to begin
		 * \param[in] size maximum number of bytes to return
		 * \return An owning buffer containing up to size bytes; an empty buffer
		 * indicates a zero-size request or end of file.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * A request of zero bytes returns without camera I/O.
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * bytes in folder/fileName or a size not representable by std::vector.
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw std::runtime_error if the driver reports more bytes than
		 * requested
		 */
		std::vector<char> fileRead(std::string const & folder,
			std::string const & fileName, CameraFileTypeWrapper const & fileType,
			std::uint64_t offset, std::uint64_t size) const;

		/**
		 * \brief Retrieves metadata for a camera file and its preview and audio.
		 * \param[in] folder folder containing the file
		 * \param[in] fileName file name within the folder
		 * \return Owned metadata; each component has its own field-validity mask.
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * libgphoto2 may fetch a preview when the driver lacks metadata support.
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * bytes in folder/fileName
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 * \throw std::runtime_error for an unterminated reported MIME type
		 */
		CameraFileInfo fileGetInfo(std::string const & folder,
			std::string const & fileName) const;

		/**
		 * \brief Requests changes to normal-file permissions or modification time.
		 * \param[in] folder folder containing the file
		 * \param[in] fileName file name within the folder
		 * \param[in] update explicitly selected writable fields
		 * \note Camera I/O is serialized with other CameraWrapper operations.
		 * Driver support varies; a driver may ignore a requested field even
		 * when it reports success. An error may follow a partial update;
		 * callers should re-read metadata before retrying.
		 * \throw GPhoto2pp::exceptions::ArgumentException for embedded NUL
		 * bytes in folder/fileName, an empty update, or unsupported permission
		 * bits
		 * \throw GPhoto2pp::exceptions::gphoto2_exception on libgphoto2 failure
		 */
		void fileSetInfo(std::string const & folder, std::string const & fileName,
			CameraFileInfoUpdate const & update) const;
		
		/**
		 * \brief Delete a file from the camera
		 * \param[in]	folder	containing the file to delete
		 * \param[in]	fileName	of the file to delete
		 * \note Direct wrapper for <tt>gp_camera_file_get(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void fileDelete(std::string const & folder, std::string const & fileName) const;
		
		/**
		 * \brief Helper method used to subscribe to Camera Wait For events.
		 * This method should be used to setup all the callbacks necessary
         * before calling startListeningForEvents. Provide your callback method
         * with lambdas or static/instance methods (use std::bind).
		 * \par Event Types:
		 * - FileAdded
		 * - FolderAdded
		 * - FileChanged
		 * - CaptureComplete
		 * - Timeout
		 * - Unknown
		 * \param[in]	event	type to subscribe to
		 * \param[in]	func	callback which will be called each time the
         *                      event type is triggered
		 * \note Helper structure that wraps
         *       <tt>gp_camera_wait_for_event(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		observer::Registration subscribeToCameraEvent(CameraEventTypeWrapper const & event, std::function<void(const CameraFilePathWrapper&, const std::string&)> func);
		
		/**
		 * \brief Starts monitoring the camera events
		 * You must subscribe to at least one event type and then perform some
         * action on the camera to see this in action.
		 * \return true if started listening for events, false if already
         * listening to events
		 */
		bool startListeningForEvents();
		
		/**
		 * \brief Signals the thread to stop listening.
		 * If there is no thread, it leaves the method immediately, if there is
         * a thread running, it is a blocking call which will not return until
         * the thread exits.
		 */
		void stopListeningForEvents();
		
	private:
		std::unique_ptr<CameraWidgetWrapper> wrapConfigWidget(
			gphoto2::_CameraWidget *widget) const;

		/**
		 * \brief Initializes the camera by connecting to the first camera found.
		 */
		void initialize();
		
		/**
		 * \brief Initializes a specific camera. You can obtain the model and
         * port using the gphoto2pp::helper::autoDetectAll
		 * \param[in]	model	of the camera to connect to
		 * \param[in]	port	to find the camera and connect to it through
		 */
		void initialize(std::string const & model, std::string const & port);

		void installTimeoutCallbacks();
		void clearTimeoutCallbacks();
		
		gphoto2::_Camera* m_camera = nullptr;
		
		ContextWrapper m_context;
		std::shared_ptr<detail::CameraTimeoutCallbackState> m_timeoutState;
		bool m_cameraInitialized = false;

		std::string m_model;
		std::string m_port;
		
		observer::SubjectEvent<CameraEventTypeWrapper, void(const CameraFilePathWrapper&, const std::string&)> m_cameraEvents;
		
		std::atomic<bool> m_listenForEvents;
		std::future<bool> m_listenForEventSignalFuture;
		std::mutex m_eventListenerMutex;
		
		mutable std::mutex m_cameraIOMutex;
	};

}

#endif // CAMERA_HPP
