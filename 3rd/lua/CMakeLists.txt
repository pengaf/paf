file(GLOB SourceFiles1
    "lua/*.c"
	)

foreach(SourceFile IN LISTS SourceFiles1)
	string(FIND ${SourceFile} "luac.c" Position1)
	string(FIND ${SourceFile} "lua.c" Position2)
	if(${Position1} EQUAL -1 AND ${Position2} EQUAL -1 )
		string(APPEND SourceFiles ${SourceFile})
		string(APPEND SourceFiles ";")
	endif()
endforeach()


set(AllFiles
    ${SourceFiles}
)

add_definitions(-DLUA_BUILD_AS_DLL)
add_library(lua SHARED ${AllFiles})
