find_package(CxxTest)

if(CXXTEST_FOUND)
#set(CXXTEST_USE_PYTHON true)
#enable_testing()

#First copies the testing png to the ../build/tests folder where all the test runners (*.cc) files are generated. That's where ctest executes it's tests from
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unit_test_sample_input.jpg DESTINATION ${PROJECT_BINARY_DIR}/tests)

#Now we set the output path for our cxxtest binaries that are generated
set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/cxxtests)

include_directories(${GPHOTO2PP_SOURCE_DIR}/gphoto2pp/include)
include_directories(${CXXTEST_INCLUDE_DIR})

CXXTEST_ADD_TEST( Helpers_gphoto2_NoDevice Helpers_gphoto2_NoDevice.cc ${CMAKE_CURRENT_SOURCE_DIR}/Helpers_gphoto2_NoDevice.h )
target_link_libraries( Helpers_gphoto2_NoDevice gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( Helpers_gphoto2_Generic Helpers_gphoto2_Generic.cc ${CMAKE_CURRENT_SOURCE_DIR}/Helpers_gphoto2_Generic.h )
target_link_libraries( Helpers_gphoto2_Generic gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( Helpers_camera_wrapper_Generic Helpers_camera_wrapper_Generic.cc ${CMAKE_CURRENT_SOURCE_DIR}/Helpers_camera_wrapper_Generic.h )
target_link_libraries( Helpers_camera_wrapper_Generic gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( Helpers_widgets_Generic Helpers_widgets_Generic.cc ${CMAKE_CURRENT_SOURCE_DIR}/Helpers_widgets_Generic.h )
target_link_libraries( Helpers_widgets_Generic gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( CameraListWrapper_NoDevice CameraListWrapper_NoDevice.cc ${CMAKE_CURRENT_SOURCE_DIR}/CameraListWrapper_NoDevice.h )
target_link_libraries( CameraListWrapper_NoDevice gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( CameraWrapper_Generic CameraWrapper_Generic.cc ${CMAKE_CURRENT_SOURCE_DIR}/CameraWrapper_Generic.h )
target_link_libraries( CameraWrapper_Generic gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( CameraFileWrapper_NoDevice CameraFileWrapper_NoDevice.cc ${CMAKE_CURRENT_SOURCE_DIR}/CameraFileWrapper_NoDevice.h )
target_link_libraries( CameraFileWrapper_NoDevice gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( CameraWidgetWrapper_Generic CameraWidgetWrapper_Generic.cc ${CMAKE_CURRENT_SOURCE_DIR}/CameraWidgetWrapper_Generic.h )
target_link_libraries( CameraWidgetWrapper_Generic gphoto2pp ${LIBS})

#CXXTEST_ADD_TEST( Generic_TestSuite Generic_TestSuite.cc ${CMAKE_CURRENT_SOURCE_DIR}/Generic_TestSuite.h )
#target_link_libraries( Generic_TestSuite gphoto2pp ${LIBS})

CXXTEST_ADD_TEST( NikonD90 NikonD90.cc ${CMAKE_CURRENT_SOURCE_DIR}/NikonD90.h )
target_link_libraries( NikonD90 gphoto2pp ${LIBS})

endif()
