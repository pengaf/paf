#import "Type.i"

namespace paf
{

#{
	class Variant;
	typedef ErrorCode(*InvokeMethod)(Variant* result, Variant** args, uint32_t numArgs);
#}

	abstract class(function_argument)#PAFCORE_EXPORT MethodArgument : Metadata
	{
		Type* type { get};
		TypeCompound typeCompound{ get };
		Passing passing{ get };
#{
	public:
		MethodArgument(const char* name, Type* type, TypeCompound typeCompound, Passing passing);
	public:
		Type* m_type;
		TypeCompound m_typeCompound;
		Passing m_passing;
#}
	};

	abstract class(function_result)#PAFCORE_EXPORT MethodResult : Metadata
	{
		Type* type { get };
		TypeCompound typeCompound{ get };
#{
	public:
		MethodResult(Type* type, TypeCompound typeCompound);
	public:
		Type* m_type;
		TypeCompound m_typeCompound;
#}
	};

	abstract class(instance_method)#PAFCORE_EXPORT InstanceMethod : Metadata
	{
		MethodResult* getResult();
		uint32_t getOutputArgumentCount();
		MethodResult* getOutputArgument(uint32_t index);
		uint32_t getArgumentCount();
		MethodArgument* getArgument(uint32_t index);
		uint32_t firstDefaultArgument();
#{
	public:
		InstanceMethod(const char* name, Attributes* attributes, InvokeMethod invokeMethod, MethodResult* result, MethodResult* outputArgs, uint32_t outputArgCount, MethodArgument* args, uint32_t argCount, uint32_t firstDefaultArg);
	public:
		InvokeMethod m_invokeMethod;
		MethodResult* m_result;
		MethodResult* m_outputArgs;
		MethodArgument* m_args;
		uint32_t m_outputArgCount;
		uint32_t m_argCount;
		uint32_t m_firstDefaultArg;
#}
	};


	abstract class(static_method)#PAFCORE_EXPORT StaticMethod : Metadata
	{
		MethodResult * getResult();
		uint32_t getOutputArgumentCount();
		MethodResult* getOutputArgument(uint32_t index);
		uint32_t getArgumentCount();
		MethodArgument* getArgument(uint32_t index);
		uint32_t firstDefaultArgument();
#{
	public:
		StaticMethod(const char* name, Attributes* attributes, InvokeMethod invokeMethod, MethodResult* result, MethodResult* outputArgs, uint32_t outputArgCount, MethodArgument* args, uint32_t argCount, uint32_t firstDefaultArg);
	public:
		InvokeMethod m_invokeMethod;
		MethodResult* m_result;
		MethodResult* m_outputArgs;
		MethodArgument* m_args;
		uint32_t m_outputArgCount;
		uint32_t m_argCount;
		uint32_t m_firstDefaultArg;
#}
	};


#{

	inline paf::Type* MethodArgument::get_type() const
	{
		return m_type;
	}

	inline Passing MethodArgument::get_passing() const
	{
		return m_passing;
	}

	inline TypeCompound MethodArgument::get_typeCompound() const
	{
		return m_typeCompound;
	}

	inline Type* MethodResult::get_type() const
	{
		return m_type;
	}

	inline TypeCompound MethodResult::get_typeCompound() const
	{
		return m_typeCompound;
	}

	inline MethodResult* InstanceMethod::getResult()
	{
		return m_result;
	}

	inline uint32_t InstanceMethod::getOutputArgumentCount()
	{
		return m_outputArgCount;
	}

	inline MethodResult* InstanceMethod::getOutputArgument(uint32_t index)
	{
		if (index < m_outputArgCount)
		{
			return &m_outputArgs[index];
		}
		return nullptr;
	}

	inline uint32_t InstanceMethod::getArgumentCount()
	{
		return m_argCount;
	}

	inline MethodArgument* InstanceMethod::getArgument(uint32_t index)
	{
		if (index < m_argCount)
		{
			return &m_args[index];
		}
		return nullptr;
	}

	inline uint32_t InstanceMethod::firstDefaultArgument()
	{
		return m_firstDefaultArg;
	}

	inline MethodResult* StaticMethod::getResult()
	{
		return m_result;
	}

	inline uint32_t StaticMethod::getOutputArgumentCount()
	{
		return m_outputArgCount;
	}

	inline MethodResult* StaticMethod::getOutputArgument(uint32_t index)
	{
		if (index < m_outputArgCount)
		{
			return &m_outputArgs[index];
		}
		return nullptr;
	}

	inline uint32_t StaticMethod::getArgumentCount()
	{
		return m_argCount;
	}

	inline MethodArgument* StaticMethod::getArgument(uint32_t index)
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