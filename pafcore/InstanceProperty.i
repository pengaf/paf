#import "ClassType.i"

namespace pafcore
{

#{
	struct Attributes;
	class Iterator;
	class Variant;
	
	typedef ErrorCode(*SimpleInstancePropertyGet)(Variant* that, Variant* value);
	typedef ErrorCode(*SimpleInstancePropertySet)(Variant* that, Variant* value);

	typedef ErrorCode(*ArrayInstancePropertySize)(Variant* that, size_t& index);
	typedef ErrorCode(*ArrayInstancePropertyGet)(Variant* that, size_t index, Variant* value);
	typedef ErrorCode(*ArrayInstancePropertySet)(Variant* that, size_t index, Variant* value);

	typedef ErrorCode(*CollectionInstancePropertyIterate)(Variant* that, Iterator*& iterator);
	typedef ErrorCode(*CollectionInstancePropertyGet)(Variant* that, Iterator* iterator, Variant* value);
	typedef ErrorCode(*CollectionInstancePropertySet)(Variant* that, Iterator* iterator, size_t removeCount, Variant* value);
#}

	abstract class(instance_property)#PAFCORE_EXPORT InstanceProperty : Metadata
	{
		ClassType* objectType { get };
		Type* type { get };
		TypeCompound getterTypeCompound{ get };
		TypeCompound setterTypeCompound{ get };
		PropertyCategory propertyCategory{ get };
#{
	public:
		InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
			SimpleInstancePropertyGet get, SimpleInstancePropertySet set);
		
		InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
			ArrayInstancePropertySize size, ArrayInstancePropertyGet get, ArrayInstancePropertySet set);

		InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
			CollectionInstancePropertyIterate iterate, CollectionInstancePropertyGet get, CollectionInstancePropertySet set);

	public:
		ClassType * m_objectType;
		Type* m_type;
		TypeCompound m_getterTypeCompound;
		TypeCompound m_setterTypeCompound;
		PropertyCategory m_propertyCategory;
		union
		{
			struct
			{
				SimpleInstancePropertyGet m_simpleGet;
				SimpleInstancePropertySet m_simpleSet;
			};
			struct
			{
				ArrayInstancePropertySize m_arraySize;
				ArrayInstancePropertyGet m_arrayGet;
				ArrayInstancePropertySet m_arraySet;
			};
			struct
			{
				CollectionInstancePropertyIterate m_collectionIterate;
				CollectionInstancePropertyGet m_collectionGet;
				CollectionInstancePropertySet m_collectionSet;
			};
		};
#}
	};

#{
	inline ClassType* InstanceProperty::get_objectType() const
	{
		return m_objectType;
	}

	inline Type* InstanceProperty::get_type() const
	{
		return m_type;
	}

	inline TypeCompound InstanceProperty::get_getterTypeCompound() const
	{
		return m_getterTypeCompound;
	}

	inline TypeCompound InstanceProperty::get_setterTypeCompound() const
	{
		return m_setterTypeCompound;
	}

	inline PropertyCategory InstanceProperty::get_propertyCategory() const
	{
		return m_propertyCategory;
	}
#}

}