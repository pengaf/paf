
set(HeaderFiles
    "AutoRun.h"
    "Debug.h"
    "DebugHelper.h"
    "RefCount.h"
    "RefCountImpl.h"
    "RefPtr.h"
    "SmartPtr.h"
    "SubclassInvoker.h"
    "Utility.h"
    "Variant.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(InterfaceFiles
    "Argument.i"
    "Buffer.i"
    "ClassType.i"
    "Delegate.i"
    "DynamicObject.i"
    "Enumerator.i"
    "EnumType.i"
    "InstanceField.i"
    "InstanceMethod.i"
    "InstanceProperty.i"
    "Iterator.i"
    "Metadata.i"
    "NameSpace.i"
    "NotifyHandler.i"
    "NotifyHandlerList.i"
    "Object.i"
    "PrimitiveType.i"
    "Reference.i"
    "Reflection.i"
    "Result.i"
    "StaticField.i"
    "StaticMethod.i"
    "StaticProperty.i"
    "String.i"
    "System.i"
    "Type.i"
    "TypeAlias.i"
    "Typedef.i"
    "VoidType.i"
)
source_group("Interface Files" FILES ${InterfaceFiles})

set(SourceFiles
    "Argument.cpp"
    "Buffer.cpp"
    "ClassType.cpp"
    "DebugHelper.cpp"
    "Delegate.cpp"
    "dllmain.cpp"
    "DynamicObject.cpp"
    "Enumerator.cpp"
    "EnumType.cpp"
    "InstanceField.cpp"
    "InstanceMethod.cpp"
    "InstanceProperty.cpp"
    "Iterator.cpp"
    "Metadata.cpp"
    "NameSpace.cpp"
    "NotifyHandler.cpp"
    "NotifyHandlerList.cpp"
    "Object.cpp"
    "PrimitiveType.cpp"
    "Reference.cpp"
    "Reflection.cpp"
    "Result.cpp"
    "SmartPtr.cpp"
    "StaticField.cpp"
    "StaticMethod.cpp"
    "StaticProperty.cpp"
    "String.cpp"
    "System.cpp"
    "Type.cpp"
    "TypeAlias.cpp"
    "Typedef.cpp"
    "Utility.cpp"
    "Variant.cpp"
    "VoidType.cpp"
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
