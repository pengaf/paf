#include "pafpy.h"
#include "PythonWrapper.h"
#include <Windows.h>

PyMODINIT_FUNC PyInit_pafpy()
{
	return pafpy::PyInit_pafpy_();
}


BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (DLL_PROCESS_ATTACH == dwReason)
	{
		OutputDebugStringA("pafpy Process Attach\n");
	}
	else if (DLL_PROCESS_DETACH == dwReason)
	{
		OutputDebugStringA("pafpy Process Detach\n");
	}
	return TRUE;
}
