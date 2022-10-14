#import "ClassType.i"

namespace pafcore
{

#{
	struct Attributes;
#}

	abstract class(instance_field)#PAFCORE_EXPORT InstanceField : Metadata
	{
		ClassType objectType { get*};
		Type type { get* };
		TypeCompound typeCompound { get };
		uint32_t arraySize{ get };
		uint32_t offset { get };
#{
	public:
		InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound typeCompound, size_t arraySize, size_t offset);
	public:
		ClassType* m_objectType;
		Type* m_type;
		TypeCompound m_typeCompound;
		uint32_t m_offset;
		uint32_t m_arraySize;
#}
	};

#{
	inline RawPtr<ClassType> InstanceField::get_objectType() const
	{
		return m_objectType;
	}

	inline RawPtr<Type> InstanceField::get_type() const
	{
		return m_type;
	}

	inline TypeCompound InstanceField::get_typeCompound() const
	{
		return m_typeCompound;
	}

	inline uint32_t InstanceField::get_arraySize() const
	{
		return m_arraySize;
	}

	inline uint32_t InstanceField::get_offset() const
	{
		return m_offset;
	}
#}
}