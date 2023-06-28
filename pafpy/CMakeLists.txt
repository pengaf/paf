set(PYTHON_DIR /python CACHE PATH "python dir") 

set(PYTHON_KEY_FILE $CACHE{PYTHON_DIR}/include/Python.h)
if(NOT EXISTS ${PYTHON_KEY_FILE})
	message("invalid entry PYTHON_DIR : " ${PYTHON_DIR})
	message("can not find file: " ${PYTHON_KEY_FILE})
	return()
endif()

set(HeaderFiles
    "pafpy.h"
	"PythonWrapper.h"
    "PythonSubclassInvoker.h"
    "Utility.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "pafpy.cpp"
	"PythonWrapper.cpp"
    "PythonSubclassInvoker.cpp"
)
source_group("Source Files" FILES ${SourceFiles})
       
set(AllFiles
    ${HeaderFiles}
    ${SourceFiles}
)

set(CMAKE_DEBUG_POSTFIX)

include_directories(${PYTHON_DIR}/include)
link_directories(${PYTHON_DIR}/libs)

add_library(pafpy SHARED ${AllFiles})
#target_compile_definitions(paflua PUBLIC _UNICODE)
target_compile_definitions(pafpy PRIVATE PAFPY_EXPORTS)


target_link_libraries(pafpy pafcore python310)
