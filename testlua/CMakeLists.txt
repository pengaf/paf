
set(HeaderFiles
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "testlua.cpp"
    "testlua1.lua"
    "testlua2.lua"
)
source_group("Source Files" FILES ${SourceFiles})

set(AllFiles
    ${HeaderFiles}
    ${SourceFiles}
)

add_executable(testlua ${AllFiles})
target_link_libraries(testlua pafcore paflua lua)
set_target_properties(testlua PROPERTIES DEBUG_POSTFIX _d)

if(WIN32)   
add_definitions( /Zi )
target_link_options(testlua PUBLIC /DEBUG)
endif(WIN32)
