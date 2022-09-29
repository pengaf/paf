
set(HeaderFiles
    "Node.h"
    "Scene.h"
)
source_group("Header Files" FILES ${HeaderFiles})
set(SourceFiles
    "Node.cpp"
    "Scene.cpp"
    "test1.cpp"
)

set(AllFiles
    ${HeaderFiles}
    ${SourceFiles}
)


source_group("Source Files" FILES ${SourceFiles})

include_directories(${PARCORE_DIR})


add_executable(test1 ${AllFiles})
target_link_libraries(test1 pafcore)
set_target_properties(test1 PROPERTIES DEBUG_POSTFIX _d)

if(WIN32)   
add_definitions( /Zi )
target_link_options(test1 PUBLIC /DEBUG)
endif(WIN32)

