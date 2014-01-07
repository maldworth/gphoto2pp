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
	
	# Gphoto 2.5 had breaking api changes that weren't compatable with older versions.
	# Although I'm developing with the latest in mind, there's no reason this wrapper
	# shouldn't work for versions less than 2.5 (as most linux distros still use 2.4.14)
	if(${GPHOTO2_VERSION_STRING} VERSION_LESS "2.5")
		add_definitions("-DGPHOTO_LESS_25")
	endif()
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

############
# gphoto2pp 
############
FILE(GLOB GPHOTO2PP_SOURCE_FILES "src/*.cpp")
FILE(GLOB GPHOTO2PP_HPPHEADER_FILES "include/*.hpp")
FILE(GLOB GPHOTO2PP_HEADER_FILES "include/*.h")

set(GPHOTO2PP_SOURCES ${GPHOTO2PP_SOURCE_FILES})
set(GPHOTO2PP_HEADERS ${GPHOTO2PP_HPPHEADER_FILES} ${GPHOTO2PP_HEADER_FILES})

add_library( gphoto2pp SHARED ${GPHOTO2PP_SOURCES})

include_directories("include")

# sets the shared library version
# http://cmake.3232098.n2.nabble.com/Version-in-name-of-shared-library-td7581530.html
set_target_properties( gphoto2pp PROPERTIES VERSION ${MYLIB_VERSION_STRING} SOVERSION ${MYLIB_VERSION_MAJOR} )

# only offer install option if they compiled with release mode
if(CMAKE_BUILD_TYPE MATCHES "RELEASE")
	# adds the 'make install' targets to copy the shared library to /usr/local/lib/ directory
	install( TARGETS gphoto2pp DESTINATION lib )
	# adds all the relevant headers to /usr/local/include/gphoto2pp when ``make install`` is executed
	install( FILES ${GPHOTO2PP_HEADERS} DESTINATION include/gphoto2pp )
endif(CMAKE_BUILD_TYPE MATCHES "RELEASE")

###########
# Examples
###########
add_subdirectory( examples )

#############
# Unit Tests
#############
# for now only compile unit tests in debug mode
enable_testing()
add_subdirectory( tests )


###############################
# Docs generation with Doxygen
###############################
find_package( Doxygen )
if( DOXYGEN_FOUND )
	add_custom_target (doc ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Generating source code documentation with Doxygen." VERBATIM)
endif() # DOXYGEN_FOUND

###################
# uninstall target
###################
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
