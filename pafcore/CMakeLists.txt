
set(HeaderFiles
    "AutoRun.h"
    "Debug.h"
    "SmartPtr.h"
    "SubclassInvoker.h"
    "Utility.h"
    "Variant.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(InterfaceFiles
    "Argument.i"
    "ClassType.i"
    "Editable.i"
    "Enumerator.i"
    "EnumType.i"
    "InstanceField.i"
    "InstanceMethod.i"
    "InstanceProperty.i"
    "Introspectable.i"
    "Iterator.i"
    "Metadata.i"
    "NameSpace.i"
    "NotifyHandler.i"
    "NotifyHandlerList.i"
    "PrimitiveType.i"
    "Reflection.i"
    "Result.i"
	"Serializable.i"
    "StaticField.i"
    "StaticMethod.i"
    "StaticProperty.i"
    "String.i"
    "System.i"
    "Type.i"
    "TypeAlias.i"
    "Utility.i"
)
source_group("Interface Files" FILES ${InterfaceFiles})

set(SourceFiles
    "Argument.cpp"
    "ClassType.cpp"
    "dllmain.cpp"
    "Editable.cpp"
    "Enumerator.cpp"
    "EnumType.cpp"
    "InstanceField.cpp"
    "InstanceMethod.cpp"
    "InstanceProperty.cpp"
    "Introspectable.cpp"
    "Iterator.cpp"
    "Metadata.cpp"
    "NameSpace.cpp"
    "NotifyHandler.cpp"
    "NotifyHandlerList.cpp"
    "PrimitiveType.cpp"
    "Reflection.cpp"
    "Result.cpp"
	"Serializable.cpp"
    "SmartPtr.cpp"
    "StaticField.cpp"
    "StaticMethod.cpp"
    "StaticProperty.cpp"
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
target_compile_definitions(pafcore PUBLIC _UNICODE)
target_compile_definitions(pafcore PUBLIC PAFCORE_EXPORTS)
