FILE(GLOB SOURCE_FILES "src/*.cpp")
FILE(GLOB HPPHEADER_FILES "include/*.hpp")
FILE(GLOB HEADER_FILES "include/*.h")

set(SOURCES ${SOURCE_FILES})
set(HEADERS ${HPPHEADER_FILES} ${HEADER_FILES})

add_library( gphoto2pp SHARED ${SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# adds the 'make install' targets to copy the shared library to /usr/local/lib/ directory
install( TARGETS gphoto2pp DESTINATION lib )
# adds all the relevant headers to /usr/local/include/gphoto2pp when make install is executed
install( FILES ${HEADERS} DESTINATION include/gphoto2pp )

if(GPHOTO2_FOUND)
	# Gphoto 2.5 had breaking api changes that weren't compatable with older versions.
	# Although I'm developing with the latest in mind, there's no reason this wrapper
	# shouldn't work for versions less than 2.5 (as most linux distros still use 2.4.14)
	if(${GPHOTO2_VERSION_STRING} VERSION_LESS "2.5")
		add_definitions("-DGPHOTO_LESS_25")
	endif()
endif(GPHOTO2_FOUND)
