#include "Utility.h"
#include "Utility.mh"
#include "Utility.ic"
#include "Utility.mc"

#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <crtdbg.h>
#include <Windows.h>
#include <string>
#include <unordered_set>
#include <mutex>

void PafAssert(wchar_t const* condition, wchar_t const* file, unsigned line, wchar_t const* format, ...)
{
#ifdef _DEBUG
	wchar_t description[1024];
	va_list args;
	va_start(args, format);
	vswprintf_s(description, format, args);
	va_end(args);
	//PafAssertionDialog(condition, file, line, description);
	_wassert(description, file, line);
	//__debugbreak();
#endif
}
