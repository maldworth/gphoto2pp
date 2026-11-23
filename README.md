gphoto2pp
=========

gphoto2pp is a C++11 wrapper for libgphoto2.

Table Of Contents
-----------------
* [Prerequisites](#prerequisites)
* [2 Minute Tutorial](#2-minute-tutorial)
* [Single-setting configuration](#single-setting-configuration)
* [Camera information text](#camera-information-text)
* [Camera port speed](#camera-port-speed)
* [Port information](#port-information)
* [Camera lifecycle](#camera-lifecycle)
* [Camera timeout callbacks](#camera-timeout-callbacks)
* [GPPort I/O](#gpport-io)
* [Context callbacks](#context-callbacks)
* [Camera abilities](#camera-abilities)
* [Streaming camera file reads](#streaming-camera-file-reads)
* [Camera file objects](#camera-file-objects)
* [Camera file metadata](#camera-file-metadata)
* [Camera storage metadata](#camera-storage-metadata)
* [Camera lists](#camera-lists)
* [Button widgets](#button-widgets)
* [Widget metadata mutation](#widget-metadata-mutation)
* [Installation](INSTALL.md#installation)
  * [User Prerequisites](INSTALL.md#user-prerequisites)
  * [Dev/Test/Contributor
    Prerequisites](INSTALL.md#devtestcontributor-prerequisites)
  * [Libgphoto2 (Simple)](INSTALL.md#installing-libgphoto2-simple)
  * [Libgphoto2 (Advanced)](INSTALL.md#installing-libgphoto2-advanced)
  * [CMake](INSTALL.md#installing-cmake)
  * [Dev/Test/Contributor Only](INSTALL.md#installing-devtestcontributor-only)
  * [gphoto2pp](INSTALL.md#installing-gphoto2pp)
* [Examples](#examples)
  * [Example 1](EXAMPLES.md#example-1)
  * [Example 2](EXAMPLES.md#example-2)
  * [Example 3](EXAMPLES.md#example-3)
  * [Example 4](EXAMPLES.md#example-4)
  * [Example 5](EXAMPLES.md#example-5)
  * [Example 6](EXAMPLES.md#example-6)
  * [Example 7](EXAMPLES.md#example-7)
  * [Example 8](EXAMPLES.md#example-8)
  * [Example 9](EXAMPLES.md#example-9)
  * [Example 10](EXAMPLES.md#example-10)
  * [Example 11](EXAMPLES.md#example-11)
* [Dev/Test/Contributor](#devtest)
* [FAQ](#faq)
* [Doxygen Documentation](#doxygen-documentation)
* [Other Useful Links](#other-useful-links)
* [Version History](#version-history)

Prerequisites
------------
* A C++11 compiler (I've only tested g++, but others should work)
* libgphoto2 (2.4.14 or greater)
* cmake (2.8.3 or greater), *only needed to build the library once to generate
  libgphoto2pp.so*
  
2 Minute Tutorial
-----------------
The following snip will show you a quick demo of how easy it is to interact with
your camera using gphoto2pp. Don't forget you will need to follow the
[installation instructions](#installation) before you can compile this 2 minute
tutorial on your computer. Make a source file eg. ``touch testgphoto2pp.cpp``
Now put this in the source file
```cpp
#include <gphoto2pp/camera_wrapper.hpp> 		// Header for CameraWrapper
#include <gphoto2pp/camera_file_wrapper.hpp>	// Header for CameraFileWrapper
#include <gphoto2pp/helper_camera_wrapper.hpp>	// Used for helper::capture(...) method

#include <iostream>

int main()
{
	// Connects to the first camera found and initializes
	gphoto2pp::CameraWrapper camera;
	
	// Prints out the summary of your camera's abilities
	std::cout << camera.getSummary() << std::endl;
	
	// Creates empty instance of a cameraFile, which will be populated in our helper method
	gphoto2pp::CameraFileWrapper cameraFile;
	
	// Takes a picture with the camera and does all of the behind the scenes fetching
	gphoto2pp::helper::capture(camera, cameraFile);
	
	// Lastly saves the picture to your hard drive
	// Your camera might take files in different formats (bmp, raw)
	// so this extension might be wrong and you should rename your file appropriately
	cameraFile.save("my-gphoto2pp-test.jpg");
	
	return 1;
}
```

Now compile with the command:
```sh
g++ -std=c++11 testgphoto2pp.cpp -lgphoto2 -lgphoto2pp
```
and then execute it
```sh
./a.out
```

That's it! The library is certainly capable of more, which you can view at
[examples](EXAMPLES.md).

Single-setting configuration
----------------------------
`CameraWrapper::listConfig()`, `getSingleConfig()`, and `setSingleConfig()`
provide access to libgphoto2's single-setting configuration APIs. Those
functions were introduced in libgphoto2 2.5.10. gphoto2pp detects their
availability at build time; when the linked version does not provide the
required functions, the corresponding call throws
`gphoto2pp::exceptions::InvalidLinkedVersionException`.

The read method returns an owning polymorphic widget. Use its widget type to
cast to a supported typed wrapper, change its local value, and pass it to the
write method:

```cpp
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/menu_widget.hpp>

gphoto2pp::CameraWrapper camera;
for (const auto &name : camera.listConfig()) {
    // Names may repeat if the camera reports duplicates.
}

auto setting = camera.getSingleConfig("iso");
auto *menu = dynamic_cast<gphoto2pp::MenuWidget *>(setting.get());
if (menu != nullptr) {
    menu->setValue("100");
    camera.setSingleConfig("iso", *setting);
}
```

`setSingleConfig()` checks that the supplied widget's name and type match the
camera's current widget and copies only its value onto the camera-provided
widget. The read and write occur under one camera-I/O lock. A driver may still
report a write error after changing the value; re-read the setting before
retrying. The returned widget owns its data and may outlive `CameraWrapper`.

Camera information text
-----------------------
`CameraWrapper::getSummary()`, `getManual()`, and `getAbout()` expose the
corresponding camera-provided libgphoto2 text. Each call serializes camera I/O
and returns an owning `std::string`. Unsupported operations and camera errors
are reported as exceptions; malformed text without a terminator in libgphoto2's
fixed-size buffer is rejected.

Camera port speed
-----------------
`CameraWrapper::getPortSpeed()` and `setPortSpeed()` expose libgphoto2's
camera-port speed operations. Camera I/O is serialized and libgphoto2 errors
are reported as exceptions. Port and driver support varies. Setting a speed
may prevent the camera driver from selecting its optimal value, so leave it
unchanged unless an application has a specific reason to manage it.

Port information
----------------
`GPPortInfoListWrapper` enumerates available port entries. `getInfo(index)`
returns an owned `PortInfoSnapshot` containing the port name, path, and type;
the copied values remain valid after the list is destroyed.
`CameraWrapper::getPortInfo()` and `setPortInfo()` expose the port information
assigned to a connected camera through the same owned snapshot type.
The list wrapper is move-only: code that previously copied it must instead
move it or pass it by reference. Rebuild consumers against the updated header;
previously compiled shallow copies remain unsafe. A moved-from wrapper may be
destroyed or assigned to, but cannot be used for lookups or enumeration.
`PortLibraryVersion()` reports the linked libgphoto2_port version; pass `true`
for verbose build information. `LibraryVersion()` reports libgphoto2's version.
`PortResultString(result)` converts a libgphoto2_port result code to owned text
and reports a null return as an error.

Camera lifecycle
----------------
`CameraWrapper::exit()` explicitly disconnects an initialized camera and stops
camera event listening before serialized I/O. The camera object remains usable
for destruction or a future reinitialization path. Repeated exit calls and
calls on a moved-from wrapper are rejected. Destruction also releases a camera
that remains initialized.
`CameraWrapper::reinitialize()` reconnects after exit using first-camera
recovery and refreshes model/port values; it rejects already initialized and
moved-from wrappers.

Camera timeout callbacks
------------------------
`CameraTimeoutCallbacks` lets a frontend provide a repeating timer scheduler
for driver keep-alive callbacks. Configure both `start` and `stop` together;
the start callback receives an interval in seconds and a C++ action that
returns a libgphoto2 result code. The returned timer ID must fit in a
nonnegative `int` unique among active timers. The stop callback must
synchronously prevent later action calls and wait for an in-flight action to
finish.

Pass the callbacks to a `CameraWrapper` constructor when a driver may request
a timer during camera initialization. `setTimeoutCallbacks()` can replace or
remove the scheduler only when no timer is active. The action may run on a
timer-owned thread, so it must not assume camera-I/O or UI-thread affinity.
`shifucc-svr` supplies its own scheduler; clients do not use gphoto2pp.
Synthetic tests verify registration, timer lifecycle and callback removal.
Keep-alive behavior on physical cameras remains unverified. Driver-only
timeout-request functions are not part of the frontend wrapper surface.

GPPort I/O
----------
`GPPortWrapper` owns a libgphoto2_port `GPPort`. It exposes lifecycle, port
information, timeout, serial/USB settings, binary read/write, interrupt reads,
serial pins, breaks, and flush operations. The wrapper validates lengths and
binary pointers through C++ types and reports libgphoto2_port errors as
exceptions. Port support is driver- and port-type-dependent; synthetic tests
do not verify physical serial or USB hardware.
`setError()` and `getError()` expose libgphoto2_port's port error text.
USB system-device selection, halt clearing, and seek operations are available
for low-level port users. These APIs require an opened/configured port and
must be used only with port types that support them.
USB discovery and standard, interface and class control transfers are also
available. Their data buffers are owned by C++ vectors and physical USB support
remains unverified.
SCSI command transfer is exposed through `sendSCSICommand()` with owned command,
sense and data buffers. It is intended for supported low-level USB-SCSI ports
and remains hardware-dependent.
`MessageCodeset()` queries the current libgphoto2 message codeset;
`MessageCodeset(codeset)` changes it and returns libgphoto2's resulting
codeset. `PortMessageCodeset()` queries the current libgphoto2_port message
codeset;
`PortMessageCodeset(codeset)` changes it and returns the previous value.
`InitializeLocaleDirectory()` configures libgphoto2's gettext locale directory,
and `InitializePortLocaleDirectory()` configures libgphoto2_port's directory.
The no-argument overload selects the compiled-in path; the overload taking a
path selects a custom directory. Call each function before the first
non-initialization operation on its library; subsequent calls may be ignored.
Builds linked against libgphoto2 versions before 2.5.31 or libgphoto2_port
versions before 0.12.1 report an unsupported-version exception.

Persisted libgphoto2 settings are available through `GetSetting()` and
`SetSetting()`. Setting identifiers, keys and values are limited to 255 bytes
to match libgphoto2's fixed-size storage. The C getter API has no buffer-size
parameter, so any custom getter registered outside gphoto2pp must honor the
same 256-byte buffer limit. gphoto2pp also provides C++ getter/setter callback
registration on linked libraries that export the callback APIs; the getter
bridge rejects values longer than 255 bytes and contains callback exceptions.
Callbacks are process-global, may run concurrently, and must be configured
before camera operations; serialize setting reads and writes.

Context callbacks
-----------------
`ContextWrapper` owns a libgphoto2 context and provides C++ callbacks for idle,
error, status, message, question, cancel, and progress notifications. It is
move-only and reports use after move as `std::logic_error`. Callback updates are
protected by a state mutex; callback bodies execute outside that mutex.
Callback bodies may run on libgphoto2 worker or camera-I/O threads, so they
must not assume UI-thread affinity and must use their own synchronization.
The wrapper also provides direct notification methods for backend code.
Direct text-reporting methods reject embedded NUL bytes; callback exceptions
are logged and converted to non-cancel feedback or a zero progress ID.
`CameraWrapper::getContext()` exposes the context owned by a camera wrapper,
allowing applications to register callbacks for that camera's operations.
For example:

```cpp
gphoto2pp::CameraWrapper camera;
auto &context = camera.getContext();
context.setCancelCallback([]() {
    return gphoto2pp::ContextFeedback::Ok;
});
context.setProgressCallbacks(
    [](float, const std::string &) { return 1U; },
    [](unsigned int, float) {},
    [](unsigned int) {});
```

Camera abilities
----------------
`CameraAbilitiesListWrapper::getAbilities()` returns an owned snapshot of a
camera model and its supported ports, serial speeds, operations, USB identity,
and device type. `loadDirectory()` loads modules from an explicit directory,
and `append()` adds a synthetic or custom abilities entry. Fixed-string limits
and embedded-NUL restrictions are enforced before libgphoto2 calls.
`CameraWrapper::getAbilities()` reads the abilities assigned to a connected
camera, and `setAbilities()` applies a validated snapshot.

Streaming camera file reads
---------------------------
`CameraWrapper::fileRead()` reads a requested byte range into an owning
`std::vector<char>`. Pass the folder, filename, file type, byte offset, and
maximum number of bytes to read. A short read (including an empty result)
indicates that the camera returned fewer bytes, such as at end of file.
Zero-size reads return an empty vector without camera I/O. Driver errors are
thrown; no whole-file buffering is required by this API.

Camera file objects
-------------------
`CameraFileWrapper` supports memory-backed data and FD-backed file ownership.
Constructing `CameraFileWrapper(fd)` transfers descriptor ownership to
libgphoto2 on successful construction; the descriptor is closed when the last
file wrapper releases the object. Negative descriptors are rejected.
`CameraFileWrapper(handlerCallbacks)` exposes libgphoto2's handler API through
C++ size/read/write callbacks. This API is marked unstable by libgphoto2.
Callback state remains valid for the lifetime of the file wrapper, and callback
exceptions are converted to libgphoto2 error codes.
`clear()` resets the file object, and `copyFrom()` copies metadata and data
between compatible file objects. Copy semantics preserve libgphoto2 reference
sharing until the wrapper lifecycle ends.
`open(filename)` imports a local file through the legacy libgphoto2 API and
rejects embedded NUL paths. It is retained for compatibility, not as the
camera-download path.

Camera file metadata
--------------------
`CameraWrapper::fileGetInfo()` returns owned metadata for the normal file,
preview, and audio components. Check each component's field mask with
`has()` before using a value. `fileSetInfo()` sends only explicitly selected
permission and modification-time fields; driver support varies. Both methods
reject embedded NUL bytes in the folder or filename.

Camera storage metadata
-----------------------
`CameraWrapper::getStorageInfo()` returns owned snapshots of the storage
entries reported by the camera. Use each entry's `has()` method to check
whether a field is valid; unsupported operations and camera errors throw.
Driver support varies.

Camera lists
------------
`CameraListWrapper::populate(format, count)` replaces a generic name/value
list with sequential entries formatted with a printf-style integer format.
The generated value is empty. Negative counts and embedded-NUL formats are
rejected before the libgphoto2 call.

Button widgets
--------------
`CameraWrapper::pressButton()` invokes the camera callback for a uniquely
named button in the current configuration. The camera may not provide button
widgets, and duplicate widget names are rejected. Button actions are not
configuration values and should not be sent with `setConfig()` or
`setSingleConfig()`. Callbacks may immediately change camera state, so use
them only when their camera-specific behavior is understood.
`ButtonWidget::hasCallback()` reports whether a callback is assigned;
`setCallback()` assigns a libgphoto2-compatible callback pointer. Clearing a
callback is not exposed because `gp_widget_set_value()` rejects a null value;
use a new standalone widget or keep the callback absent.

Widget metadata mutation
------------------------
`CameraWidgetWrapper::setName()`, `setInfo()`, `setReadOnly()`, and
`setChanged()` expose libgphoto2's metadata mutations for existing widgets.
Names and info strings reject embedded NUL bytes and fixed-buffer overflow;
read-only and changed values accept only 0 or 1. `isChanged()` reports the
current libgphoto2 changed flag. Labels remain read-only because libgphoto2
does not expose a public label setter.
`CameraWidgetWrapper::create()` builds standalone typed widgets.
`NonValueWidget::append()` and `prepend()` insert children; `ChoicesWidget`
supports `addChoice()`; `RangeWidget::setRange()` updates finite ordered ranges.

Installation
------------
Please see the [Installation document](INSTALL.md) for details.

Examples
--------
Please see the [Examples document](EXAMPLES.md) for details about all examples
provided.

Dev/Test
--------
If you would like to contribute/develop/test gphoto2pp, please see the [Dev-Test
doc](DEV-TEST.md).

FAQ
---
Please see the [FAQ document](FAQ.md)

Doxygen Documentation
-----------------------
Please see this [projects github pages](http://maldworth.github.io/gphoto2pp/)
for the auto generated Doxygen Page.

Other Useful Links
---------------
If you are new to gphoto2pp (and perhaps gphoto), then please view some of these
resources:
* This library wraps [gphoto](http://www.gphoto.org/)

Version History
---------------
Placeholder
