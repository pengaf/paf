
set(InterfaceFiles
    "testpy.i"
)
source_group("Interface Files" FILES ${InterfaceFiles})

set(HeaderFiles
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "testpy.cpp"
    "testpy1.py"
)
source_group("Source Files" FILES ${SourceFiles})

set(AllFiles
	${InterfaceFiles}
	${HeaderFiles}
	${SourceFiles}
)

foreach(InterfaceFile IN LISTS InterfaceFiles)
    string(REGEX REPLACE "\\.i$" ".h" HeaderFile ${InterfaceFile})
    #message(${HeaderFile})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${HeaderFile}
	    COMMAND ${IDLCPP_PATH} -pc${PAFCORE_DIR} "%(FullPath)"
	    MAIN_DEPENDENCY ${InterfaceFile})
endforeach()

add_definitions(-DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${PYTHON_DIR}/include)
link_directories(${PYTHON_DIR}/libs)

add_executable(testpy ${AllFiles})
target_link_libraries(testpy pafcore pafpy python310)
set_target_properties(testpy PROPERTIES DEBUG_POSTFIX _d)

if(WIN32)   
add_definitions( /Zi )
target_link_options(testpy PUBLIC /DEBUG)
endif(WIN32)
