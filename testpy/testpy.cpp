#include <tchar.h>
#include <string>
#include <windows.h>

#include "../pafpy/pafpy.h"
#include "testpy.h"
#include "testpy.mh"
#include "testpy.ic"
#include "testpy.mc"


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

	std::string path = CMAKE_CURRENT_SOURCE_DIR;
	//GetExePath(path);
	path += "/testpy1.py";

	path = "testpy1.py";


	if (PyImport_AppendInittab("pafpy", PyInit_pafpy) == -1) 
	{
		fprintf(stderr, "Error: could not extend in-built modules table\n");
		exit(1);
	}

	Py_Initialize();
	PyImport_ImportModule(path.c_str());
	//PyObject* pName = PyUnicode_FromString(path.c_str());
	//PyObject* pModule = PyImport_Import(pName);
	//Py_DECREF(pName);
	//if (pModule != NULL)
	//{
	//	Py_DECREF(pModule);
	//}
	//else
	//{
	//	PyErr_Print();
	//	fprintf(stderr, "Failed to load \"%s\"\n", path.c_str());
	//}
	Py_Finalize();

	//int error;
	//lua_State *L = luaL_newstate();
	//luaL_openlibs(L);
	//luaopen_paflua(L);
	//error = luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0);
	//if (error) 
	//{
	//	fprintf(stderr, "%s\n", lua_tostring(L, -1));
	//	lua_pop(L, 1);
	//}
	//lua_close(L);
	return 0;
}

