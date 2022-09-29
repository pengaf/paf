cmake_minimum_required(VERSION 3.0)
project(pafcore)


set(IDLCPP_PATH "C:/idlcpp/bin/idlcpp.exe" CACHE FILEPATH "idlcpp path")

if(NOT EXISTS ${IDLCPP_PATH})
	message("can not find file: " ${IDLCPP_PATH})
	return()
endif()


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED false)

set(PAFCORE_DIR ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_DEBUG_POSTFIX _d)

add_subdirectory(pafcore)
add_subdirectory(test)