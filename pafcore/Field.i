#import "ClassType.i"

namespace paf
{
	abstract class(instance_field)#PAFCORE_EXPORT InstanceField : Metadata
	{
		ClassType objectType { get*};
		Type type { get* };
		TypeCompound typeCompound { get };
		uint32_t arraySize{ get };
		uint32_t offset { get };
#{
	public:
		InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound typeCompound, uint32_t arraySize, uint32_t offset);
	public:
		bool isArray() const
		{
			return m_arraySize > 0;
		}
	private:
		ClassType* m_objectType;
		Type* m_type;
		TypeCompound m_typeCompound;
		uint32_t m_offset;
		uint32_t m_arraySize;
#}
	};

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
		bool isArray() const
		{
			return m_arraySize > 0;
		}
	private:
		Type* m_type;
		TypeCompound m_typeCompound;
		uint32_t m_arraySize;
		size_t m_address;
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