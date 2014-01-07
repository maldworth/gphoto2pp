set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/examples)

FILE(GLOB EXAMPLES_SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
foreach( examplesourcefile ${EXAMPLES_SOURCE_FILES} )
    # I used a simple string replace, to cut off .cpp.
    string( REPLACE ".cpp" "" examplename ${examplesourcefile} )
    add_executable( ${examplename} ${examplesourcefile} )
    # Make sure YourLib is linked to each app
    target_link_libraries( ${examplename} gphoto2pp ${LIBS} )
endforeach( examplesourcefile ${EXAMPLES_SOURCE_FILES} )
