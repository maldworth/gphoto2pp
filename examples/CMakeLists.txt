set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/examples)

add_custom_target( examples )

FILE(GLOB EXAMPLES_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
foreach( examplesourcefile ${EXAMPLES_SOURCE_FILES} )
    # I used a simple string replace, to cut off .cpp.
    string( REPLACE ".cpp" "" examplename ${examplesourcefile} )
    add_executable( ${examplename} EXCLUDE_FROM_ALL ${examplesourcefile} )
    # Make sure YourLib is linked to each app
    target_link_libraries( ${examplename} gphoto2pp ${LIBS} )
    # Adds this example to the `make examples` custom target
    add_dependencies( examples ${examplename} )
endforeach( examplesourcefile ${EXAMPLES_SOURCE_FILES} )
