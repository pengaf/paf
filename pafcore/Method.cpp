#include "Method.h"
#include "Method.mh"
#include "Method.ic"
#include "Method.mc"

BEGIN_PAF

MethodArgument::MethodArgument(const char* name, Type* type, TypeCompound typeCompound, Passing passing) :
	Metadata(name)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_passing = passing;
}

MethodResult::MethodResult(Type* type, TypeCompound typeCompound)
	: Metadata(nullptr)
{
	m_type = type;
	m_typeCompound = typeCompound;
}

InstanceMethod::InstanceMethod(const char* name, Attributes* attributes, InvokeMethod invokeMethod, MethodResult* result, MethodResult* outputArgs, uint32_t outputArgCount, MethodArgument* args, uint32_t argCount, uint32_t firstDefaultArg) :
	Metadata(name, attributes)
{
	m_invokeMethod = invokeMethod;
	m_result = result;
	m_outputArgs = outputArgs;
	m_outputArgCount = outputArgCount;
	m_args = args;
	m_argCount = argCount;
	m_firstDefaultArg = firstDefaultArg;
}

StaticMethod::StaticMethod(const char* name, Attributes* attributes, InvokeMethod invokeMethod, MethodResult* result, MethodResult* outputArgs, uint32_t outputArgCount, MethodArgument* args, uint32_t argCount, uint32_t firstDefaultArg) :
	Metadata(name, attributes)
{
	m_invokeMethod = invokeMethod;
	m_result = result;
	m_args = args;
	m_argCount = argCount;
	m_firstDefaultArg = firstDefaultArg;
}

END_PAF