#include "Utility.h"
#include "Utility.mh"
#include "Utility.ic"
#include "Utility.mc"

#include <stdarg.h>
#include <assert.h>
//#include <malloc.h>
//#include <crtdbg.h>
//#include <Windows.h>
//#include <string>
//#include <unordered_set>
//#include <mutex>

void PafAssert(wchar_t const* condition, wchar_t const* file, unsigned line, wchar_t const* format, ...)
{
#ifdef _DEBUG
	wchar_t description[4096];
	va_list args;
	va_start(args, format);
	vswprintf_s(description, format, args);
	va_end(args);
	//PafAssertionDialog(condition, file, line, description);
	_wassert(description, file, line);
	//__debugbreak();
#endif
}

const char* g_errorStrings[] =
{
	"s_ok",
	"e_name_conflict",
	"e_is_not_type",
	"e_is_not_class",
	"e_is_not_string",	
	"e_is_not_array",
	"e_invalid_subscript_type",
	"e_member_not_found",
	"e_index_out_of_range",
	"e_is_not_property",
	"e_is_not_array_property",
	"e_is_not_collection_property",
	"e_property_get_not_implemented",
	"e_property_set_not_implemented",
	"e_property_size_not_implemented",
	"e_property_iterate_not_implemented",
	"e_field_is_an_array",
	"e_invalid_type",
	"e_invalid_object_type",
	"e_invalid_field_type",
	"e_invalid_property_type",
	"e_invalid_this_type",
	"e_invalid_arg_type_1",
	"e_invalid_arg_type_2",
	"e_invalid_arg_type_3",
	"e_invalid_arg_type_4",
	"e_invalid_arg_type_5",
	"e_invalid_arg_type_6",
	"e_invalid_arg_type_7",
	"e_invalid_arg_type_8",
	"e_invalid_arg_type_9",
	"e_invalid_arg_type_10",
	"e_invalid_arg_type_11",
	"e_invalid_arg_type_12",
	"e_invalid_arg_type_13",
	"e_invalid_arg_type_14",
	"e_invalid_arg_type_15",
	"e_invalid_arg_type_16",
	"e_invalid_arg_type_17",
	"e_invalid_arg_type_18",
	"e_invalid_arg_type_19",
	"e_invalid_arg_type_20",
	"e_invalid_arg_type_21",
	"e_invalid_arg_type_22",
	"e_invalid_arg_type_23",
	"e_invalid_arg_type_24",
	"e_invalid_arg_type_25",
	"e_invalid_arg_type_26",
	"e_invalid_arg_type_27",
	"e_invalid_arg_type_28",
	"e_invalid_arg_type_29",
	"e_invalid_arg_type_30",
	"e_invalid_arg_type_31",
	"e_invalid_arg_type_32",
	"e_too_few_arguments",
	"e_too_many_arguments",
	"e_too_few_results",
	"e_not_implemented",
	"e_script_error",
	"e_script_dose_not_override",
};

BEGIN_PAF

const char* ErrorCodeToString(ErrorCode errorCode)
{
	if (size_t(errorCode) < paf_array_size_of(g_errorStrings))
	{
		return g_errorStrings[size_t(errorCode)];
	}
	return "unknown error";
}

bool Introspectable::isTypeOf(ClassType* classType)
{
	ClassType* thisType = getType();
	return thisType->isType(classType);
}

void* Introspectable::castTo(ClassType* classType)
{
	size_t offset;
	ClassType* thisType = getType();
	if (thisType->getClassOffset(offset, classType))
	{
		void* address = getMemoryAddress();
		return (void*)((size_t)address + offset);
	}
	return 0;
}

END_PAF
