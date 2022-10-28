
set(HeaderFiles
	"ArrayFieldWrapper.h"
	"ArrayPropertyWrapper.h"
	"CollectionPropertyWrapper.h"
    "LuaSubclassInvoker.h"
    "paflua.h"
    "Utility.h"
    "VariantWrapper.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
	"ArrayFieldWrapper.cpp"
	"ArrayPropertyWrapper.cpp"
	"CollectionPropertyWrapper.cpp"
    "LuaSubclassInvoker.cpp"
    "paflua.cpp"
    "Utility.cpp"
    "VariantWrapper.cpp"
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
