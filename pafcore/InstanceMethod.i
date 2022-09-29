#import "Argument.i"
#import "Result.i"

#{
#include "Argument.h"
#}

namespace pafcore
{

#{
	struct Attributes;
#}

	abstract class(instance_method)#PAFCORE_EXPORT InstanceMethod : Metadata
	{
		Result* getResult();
		uint32_t getArgumentCount();
		Argument* getArgument(uint32_t index);
		uint32_t firstDefaultArgument();
#{
	public:
		InstanceMethod(const char* name, Attributes* attributes, FunctionInvoker invoker, Result* result, Argument* args, uint32_t argCount, uint32_t firstDefaultArg);
	public:
		FunctionInvoker m_invoker;
		Result* m_result;
		Argument* m_args;
		uint32_t m_argCount;
		uint32_t m_firstDefaultArg;
#}
	};

#{
	inline Result* InstanceMethod::getResult()
	{
		return m_result;
	}

	inline uint32_t InstanceMethod::getArgumentCount()
	{
		return m_argCount;
	}

	inline Argument* InstanceMethod::getArgument(uint32_t index)
	{
		if (index < m_argCount)
		{
			return &m_args[index];
		}
		return nullptr;
	}
#}

}