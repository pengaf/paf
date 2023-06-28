cmake_minimum_required(VERSION 3.0)
project(paf)


set(IDLCPP_PATH "C:/idlcpp/bin/idlcpp.exe" CACHE FILEPATH "idlcpp path")
set(PAFCORE_DIR "C:/paf/pafcore" CACHE PATH "pafcore dir") 

if(NOT EXISTS ${IDLCPP_PATH})
	message("can not find file: " ${IDLCPP_PATH})
	return()
endif()

set(PAFCORE_KEY_FILE $CACHE{PAFCORE_DIR}/Variant.h)
if(NOT EXISTS ${PAFCORE_KEY_FILE})
	message("invalid entry PAFCORE_DIR : " ${PAFCORE_DIR})
	message("can not find file: " ${PAFCORE_KEY_FILE})
	return()
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED false)

#set(PAFCORE_DIR ${PROJECT_SOURCE_DIR})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_DEBUG_POSTFIX _d)

add_subdirectory(3rd/lua)
add_subdirectory(pafcore)
add_subdirectory(paflua)
add_subdirectory(pafpy)
#add_subdirectory(pafchakra)
add_subdirectory(test)
add_subdirectory(testlua)
add_subdirectory(testpy)
