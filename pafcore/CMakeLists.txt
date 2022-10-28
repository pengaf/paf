
set(HeaderFiles
	"SmartPtr.h"
	"Variant.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(InterfaceFiles
	"ClassType.i"
	"Editable.i"
	"EnumType.i"
	"Field.i"
	"Metadata.i"
	"Method.i"
	"NameSpace.i"
	"NotifyHandler.i"
	"PrimitiveType.i"
	"Property.i"
	"Reflection.i"
	"Serializable.i"
	"String.i"
	"System.i"
	"Type.i"
	"TypeAlias.i"
	"Utility.i"
)
source_group("Interface Files" FILES ${InterfaceFiles})

set(SourceFiles
	"ClassType.cpp"
	"dllmain.cpp"
	"Editable.cpp"
	"EnumType.cpp"
	"Field.cpp"
	"Metadata.cpp"
	"Method.cpp"
	"NameSpace.cpp"
	"NotifyHandler.cpp"
	"PrimitiveType.cpp"
	"Property.cpp"
	"Reflection.cpp"
	"Serializable.cpp"
	"String.cpp"
	"System.cpp"
	"Type.cpp"
	"TypeAlias.cpp"
	"Utility.cpp"
	"Variant.cpp"
)
source_group("Source Files" FILES ${SourceFiles})
    
#message(${PROJECT_SOURCE_DIR})
#message(${CMAKE_CURRENT_SOURCE_DIR})
    
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
	    COMMAND ${IDLCPP_PATH} -emPAFCORE_EXPORT "%(FullPath)"
	    MAIN_DEPENDENCY ${InterfaceFile})
endforeach()

add_library(pafcore SHARED ${AllFiles})
#target_compile_definitions(pafcore PUBLIC _UNICODE)
target_compile_definitions(pafcore PRIVATE PAFCORE_EXPORTS)
