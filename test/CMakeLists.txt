
set(HeaderFiles
)
source_group("Header Files" FILES ${HeaderFiles})


set(InterfaceFiles
    "Node.i"
    "Scene.i"
)
source_group("Interface Files" FILES ${InterfaceFiles})

set(SourceFiles
    "Node.cpp"
    "Scene.cpp"
    "test1.cpp"
)
source_group("Source Files" FILES ${SourceFiles})

set(AllFiles
    ${HeaderFiles}
    ${InterfaceFiles}
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


#include_directories(${PARCORE_DIR})

add_executable(test1 ${AllFiles})
target_link_libraries(test1 pafcore)
set_target_properties(test1 PROPERTIES DEBUG_POSTFIX _d)

if(WIN32)   
add_definitions( /Zi )
target_link_options(test1 PUBLIC /DEBUG)
endif(WIN32)
