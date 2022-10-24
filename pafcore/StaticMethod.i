#import "Argument.i"
#import "Result.i"
#{
#include "Argument.h"
#}


namespace paf
{

#{
	struct Attributes;
	class Variant;
	typedef ErrorCode(*FunctionInvoker)(Variant* result, Variant** args, uint32_t numArgs);
#}

	abstract class(static_method)#PAFCORE_EXPORT StaticMethod : Metadata
	{
		Result * getResult();
		uint32_t getArgumentCount();
		Argument* getArgument(uint32_t index);
		uint32_t firstDefaultArgument();
#{
	public:
		StaticMethod(const char* name, Attributes* attributes, FunctionInvoker invoker, Result* result, Argument* args, uint32_t argCount, uint32_t firstDefaultArg);
	public:
		FunctionInvoker m_invoker;
		Result* m_result;
		Argument* m_args;
		uint32_t m_argCount;
		uint32_t m_firstDefaultArg;
#}
	};

#{
	inline RawPtr<Result> StaticMethod::getResult()
	{
		return m_result;
	}

	inline uint32_t StaticMethod::getArgumentCount()
	{
		return m_argCount;
	}

	inline RawPtr<Argument> StaticMethod::getArgument(uint32_t index)
	{
		if (index < m_argCount)
		{
			return &m_args[index];
		}
		return nullptr;
	}

	inline uint32_t StaticMethod::firstDefaultArgument()
	{
		return m_firstDefaultArg;
	}


#}

}