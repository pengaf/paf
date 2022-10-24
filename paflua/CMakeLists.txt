
set(HeaderFiles
    "LuaWrapper.h"
    "LuaSubclassInvoker.h"
    "paflua.h"
    "Utility.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "LuaWrapper.cpp"
    "LuaSubclassInvoker.cpp"
    "paflua.cpp"
)
source_group("Source Files" FILES ${SourceFiles})
       
set(AllFiles
    ${HeaderFiles}
    ${SourceFiles}
)

add_library(paflua SHARED ${AllFiles})
#target_compile_definitions(paflua PUBLIC _UNICODE)
target_compile_definitions(paflua PRIVATE PAFLUA_EXPORTS)

target_link_libraries(paflua pafcore lua)
