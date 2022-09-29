#import "Type.i"

namespace pafcore
{

#{
	class Variant;
	typedef ErrorCode(*FunctionInvoker)(Variant* result, Variant** args, uint32_t numArgs);
#}

	abstract class(function_argument)#PAFCORE_EXPORT Argument : Metadata
	{
		Type* type { get };
		TypeCompound typeCompound{ get };
		Passing passing{ get };
#{
	public:
		Argument(const char* name, Type* type, TypeCompound typeCompound, Passing passing);
	public:
		Type* m_type;
		TypeCompound m_typeCompound;
		Passing m_passing;
#}
	};

#{
	inline Type* Argument::get_type() const
	{
		return m_type;
	}

	inline Passing Argument::get_passing() const
	{
		return m_passing;
	}
	 
	inline TypeCompound Argument::get_typeCompound() const
	{
		return m_typeCompound;
	}

#}
}