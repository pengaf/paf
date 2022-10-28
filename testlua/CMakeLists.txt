
set(InterfaceFiles
    "testlua.i"
)
source_group("Interface Files" FILES ${InterfaceFiles})

set(HeaderFiles
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "testlua.cpp"
    "testlua1.lua"
    "testlua2.lua"
    "testlua3.lua"
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

add_executable(testlua ${AllFiles})
target_link_libraries(testlua pafcore paflua lua)
set_target_properties(testlua PROPERTIES DEBUG_POSTFIX _d)

if(WIN32)   
add_definitions( /Zi )
target_link_options(testlua PUBLIC /DEBUG)
endif(WIN32)
