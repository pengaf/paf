#include <tchar.h>
#include <string>
#include <windows.h>

#include "../3rd/lua/src/lua.hpp"
#include "../paflua/paflua.h"


void GetExePath(std::string& path)
{
	char fileName[MAX_PATH];
	GetModuleFileName(0, fileName, sizeof(fileName));
	const char* end = _tcsrchr(fileName, '\\');
	path.assign((const char*)fileName, end + 1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpFlag);
	int error;
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_paflua(L);
	std::string path;
	GetExePath(path);
	path += "../testlua/testlua2.lua";
	error = luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0);
	if (error) 
	{
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	lua_close(L);
	return 0;
}

