find_package(CxxTest)
if(CXXTEST_FOUND)
	enable_testing()
	set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/cxxtests)
	
	#include_directories(${GPHOTO2PP_SOURCE_DIR}/gphoto2pp/include)
	include_directories(${CXXTEST_INCLUDE_DIR})
	
	#First copies the testing png to the ../build/tests folder where all the test runners (*.cc) files are generated. That's where ctest executes it's tests from
	file(COPY unit_test_sample_input.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
	FILE(GLOB TESTS_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
	foreach( testssourcefile ${TESTS_SOURCE_FILES} )
		# I used a simple string replace, to cut off .h.
		string( REPLACE ".h" "" testname ${testssourcefile} )
		CXXTEST_ADD_TEST( ${testname} "${testname}.cc" "${CMAKE_CURRENT_SOURCE_DIR}/${testssourcefile}" )
		# Make sure YourLib is linked to each app
		target_link_libraries( ${testname} gphoto2pp ${LIBS} )
	endforeach( testssourcefile ${TESTS_SOURCE_FILES} )
endif(CXXTEST_FOUND)
