include_directories(${GPHOTO2PP_SOURCE_DIR}/gphoto2pp/include)

#link_directories(${GPHOTO2PP_BINARY_DIR}/libs) # I don't think this is necessary, but it was in the cmake simple example...

set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/examples)

# Example 1, checks gphoto version linked in system
add_executable( example1 example1.cpp )
target_link_libraries( example1 gphoto2pp ${LIBS})

# Example 2, autodetects Cameras connected to the computer
add_executable( example2 example2.cpp )
target_link_libraries( example2 gphoto2pp ${LIBS})

# Example 3, connects to camera (first one it finds) and spits out the summary
add_executable( example3 example3.cpp )
target_link_libraries( example3 gphoto2pp ${LIBS})

# Example 4, runs the standard capture command and downloads the picture
add_executable( example4 example4.cpp )
target_link_libraries( example4 gphoto2pp ${LIBS})

# Example 5, Getting widgets from the camera! Widgets are how we interact with and control the cameras various settings
add_executable( example5 example5.cpp )
target_link_libraries( example5 gphoto2pp ${LIBS})

# Example 6, more about widgets! Learn how to get and set specific widgets
add_executable( example6 example6.cpp )
target_link_libraries( example6 gphoto2pp ${LIBS})

# Example 7, runs the preview capture
add_executable( example7 example7.cpp )
target_link_libraries( example7 gphoto2pp ${LIBS})

# Example 8, preview with manual focus changes
add_executable( example8 example8.cpp )
target_link_libraries( example8 gphoto2pp ${LIBS})

# Example 9, list files in camera
add_executable( example9 example9.cpp )
target_link_libraries( example9 gphoto2pp ${LIBS})

# Example 10, list files in camera
add_executable( example10 example10.cpp )
target_link_libraries( example10 gphoto2pp ${LIBS})

# Example 11, test moving and copy constructors
add_executable( example11 example11.cpp )
target_link_libraries( example11 gphoto2pp ${LIBS})
