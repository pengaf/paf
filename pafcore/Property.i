#import "ClassType.i"

#{
#include "SmartPtr.h"
#}

namespace paf
{

#{
	class Variant;
	class Iterator;
	class InstanceProperty;
	class StaticProperty;
	
	typedef ErrorCode(*SimpleInstancePropertyGet)(InstanceProperty* instanceProperty, Variant* that, Variant* value);
	typedef ErrorCode(*SimpleInstancePropertySet)(InstanceProperty* instanceProperty, Variant* that, Variant* value);

	typedef ErrorCode(*ArrayInstancePropertySize)(InstanceProperty* instanceProperty, Variant* that, size_t& size);
	typedef ErrorCode(*ArrayInstancePropertyGet)(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);
	typedef ErrorCode(*ArrayInstancePropertySet)(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);

	typedef ErrorCode(*CollectionInstancePropertyIterate)(InstanceProperty* instanceProperty, Variant* that, UniquePtr<Iterator>& iterator);
	typedef ErrorCode(*CollectionInstancePropertyGet)(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, Variant* value);
	typedef ErrorCode(*CollectionInstancePropertySet)(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, size_t removeCount, Variant* value);


	typedef ErrorCode(*SimpleStaticPropertyGet)(StaticProperty* staticProperty, Variant* value);
	typedef ErrorCode(*SimpleStaticPropertySet)(StaticProperty* staticProperty, Variant* value);

	typedef ErrorCode(*ArrayStaticPropertySize)(StaticProperty* staticProperty, size_t& size);
	typedef ErrorCode(*ArrayStaticPropertyGet)(StaticProperty* staticProperty, size_t index, Variant* value);
	typedef ErrorCode(*ArrayStaticPropertySet)(StaticProperty* staticProperty, size_t index, Variant* value);

	typedef ErrorCode(*CollectionStaticPropertyIterate)(StaticProperty* staticProperty, UniquePtr<Iterator>& iterator);
	typedef ErrorCode(*CollectionStaticPropertyGet)(StaticProperty* staticProperty, Iterator* iterator, Variant* value);
	typedef ErrorCode(*CollectionStaticPropertySet)(StaticProperty* staticProperty, Iterator* iterator, size_t removeCount, Variant* value);

#}

	abstract class(instance_property)#PAFCORE_EXPORT InstanceProperty : Metadata
	{
		ClassType objectType { get* };
		Type type { get* };
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


	abstract class(static_property)#PAFCORE_EXPORT StaticProperty : Metadata
	{
		Type type { get* };
		TypeCompound getterTypeCompound{ get };
		TypeCompound setterTypeCompound{ get };
		PropertyCategory propertyCategory{ get };
#{
	public:
		StaticProperty(const char* name, Attributes* attributes, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
			SimpleStaticPropertyGet get, SimpleStaticPropertySet set);

		StaticProperty(const char* name, Attributes* attributes, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
			ArrayStaticPropertySize size, ArrayStaticPropertyGet get, ArrayStaticPropertySet set);

		StaticProperty(const char* name, Attributes* attributes, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
			CollectionStaticPropertyIterate iterate, CollectionStaticPropertyGet get, CollectionStaticPropertySet set);
	public:
		Type* m_type;
		TypeCompound m_getterTypeCompound;
		TypeCompound m_setterTypeCompound;
		PropertyCategory m_propertyCategory;
		union
		{
			struct
			{
				SimpleStaticPropertyGet m_simpleGet;
				SimpleStaticPropertySet m_simpleSet;
			};
			struct
			{
				ArrayStaticPropertySize m_arraySize;
				ArrayStaticPropertyGet m_arrayGet;
				ArrayStaticPropertySet m_arraySet;
			};
			struct
			{
				CollectionStaticPropertyIterate m_collectionIterate;
				CollectionStaticPropertyGet m_collectionGet;
				CollectionStaticPropertySet m_collectionSet;
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

	inline Type* StaticProperty::get_type() const
	{
		return m_type;
	}

	inline TypeCompound StaticProperty::get_getterTypeCompound() const
	{
		return m_getterTypeCompound;
	}

	inline TypeCompound StaticProperty::get_setterTypeCompound() const
	{
		return m_setterTypeCompound;
	}

	inline PropertyCategory StaticProperty::get_propertyCategory() const
	{
		return m_propertyCategory;
	}

#}

}