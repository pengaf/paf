#import "Type.i"

namespace paf
{

#{
	struct Attributes;
#}

	abstract class(static_field)#PAFCORE_EXPORT StaticField : Metadata
	{
		Type type { get* };
		TypeCompound typeCompound{ get };
		uint32_t arraySize{ get };
		size_t address { get };
#{
	public:
		StaticField(const char* name, Attributes* attributes, Type* type, TypeCompound typeCompound, uint32_t arraySize, size_t address);
	public:
		Type* m_type;
		TypeCompound m_typeCompound;
		uint32_t m_arraySize;
		size_t m_address;
#}
	};

#{
	inline RawPtr<Type> StaticField::get_type() const
	{
		return m_type;
	}

	inline TypeCompound StaticField::get_typeCompound() const
	{
		return m_typeCompound;
	}

	inline uint32_t StaticField::get_arraySize() const
	{
		return m_arraySize;
	}

	inline size_t StaticField::get_address() const
	{
		return m_address;
	}
#}

}