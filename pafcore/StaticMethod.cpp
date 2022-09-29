#include "StaticMethod.h"
#include "StaticMethod.mh"
#include "StaticMethod.ic"
#include "StaticMethod.mc"

BEGIN_PAFCORE

StaticMethod::StaticMethod(const char* name, Attributes* attributes, FunctionInvoker invoker, Result* result, Argument* args, uint32_t argCount, uint32_t firstDefaultArg) :
	Metadata(name, attributes)
{
	m_invoker = invoker;
	m_result = result;
	m_args = args;
	m_argCount = argCount;
	m_firstDefaultArg = firstDefaultArg;
}

END_PAFCORE