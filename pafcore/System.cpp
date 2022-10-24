#include "System.h"
#include "System.ic"
#include "System.mh"
#include "System.mc"

#ifdef _WINDOWS

#include <windows.h>

namespace paf
{
	string_t System::GetProgramPath()
	{
		static char s_buffer[MAX_PATH] = {0};
		if (0 == s_buffer[0])
		{
			GetModuleFileNameA(0, s_buffer, MAX_PATH);
		}
		return s_buffer;
	}

	void System::LoadDLL(string_t fileName)
	{
		LoadLibraryA(fileName);
	}

	void System::OutputDebug(string_t str)
	{
		::OutputDebugStringA(str);
	}

	void System::DebugBreak()
	{
		::DebugBreak();
	}

	void System::Sleep(ulong_t millisecond)
	{
		::Sleep(millisecond);
	}
}

#endif
