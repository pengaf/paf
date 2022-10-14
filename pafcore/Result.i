#import "Type.i"

namespace pafcore
{
	##class Variant;

	abstract class(function_result)#PAFCORE_EXPORT Result : Metadata
	{
		Type type { get* };
		TypeCompound typeCompound{ get };
#{
	public:
		Result(Type* type, TypeCompound typeCompound);
	public:
		Type* m_type;
		TypeCompound m_typeCompound;
#}
	};

}