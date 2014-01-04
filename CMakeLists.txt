cmake_minimum_required (VERSION 2.8.3)
project (GPHOTO2PP)

if(CMAKE_BUILD_TYPE)
    message("Building ${CMAKE_BUILD_TYPE} build.")
elseif(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "No build type specified. Use -DCMAKE_BUILD_TYPE=[DEBUG|RELEASE] to specify.")
endif(CMAKE_BUILD_TYPE)

#set the version of our library
# MAJOR, is significant breaking changes, MINOR, is when we might have changed signatures of existing functions, PATCH, is just bug fixes and no breaking changes
set(MYLIB_VERSION_MAJOR 1)
set(MYLIB_VERSION_MINOR 0)
set(MYLIB_VERSION_PATCH 0)
set(MYLIB_VERSION_STRING ${MYLIB_VERSION_MAJOR}.${MYLIB_VERSION_MINOR}.${MYLIB_VERSION_PATCH})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/)

# finds the current version of libgphoto2 installed
find_package( Gphoto2 REQUIRED )
if(GPHOTO2_FOUND)
	include_directories(${Gphoto2_INCLUDE_DIRS})
	list(APPEND LIBS ${Gphoto2_LIBRARIES})
endif()

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
	if(CMAKE_BUILD_TYPE MATCHES "RELEASE")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -O2")
	elseif(CMAKE_BUILD_TYPE MATCHES "DEBUG")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
	endif(CMAKE_BUILD_TYPE MATCHES "RELEASE")
else()
  # I have never tried it on any other OS. I suspect apple will work.
  message( FATAL_ERROR "I have never tried this on a non Linux machine, please try at your own risk")
endif()

# base path of where binaries should be built
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# where the library will be built to
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/libs)

# always build the library regardless of build type
add_subdirectory( gphoto2pp )
add_subdirectory( examples )

if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
	enable_testing()
	add_subdirectory( tests )
endif(CMAKE_BUILD_TYPE MATCHES "DEBUG")

# Docs generation with Doxygen
find_package( Doxygen )
if( DOXYGEN_FOUND )
	add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Generating source code documentation with Doxygen." VERBATIM)
endif() # DOXYGEN_FOUND

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
