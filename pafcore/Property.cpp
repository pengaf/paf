#include "Property.h"
#include "Property.mh"
#include "Property.ic"
#include "Property.mc"

BEGIN_PAF

InstanceProperty::InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound typeCompound,
	SimpleInstancePropertyGet get, SimpleInstancePropertySet set) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_propertyCategory = PropertyCategory::simple_property;
	m_simpleGet = get;
	m_simpleSet = set;
}

InstanceProperty::InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound typeCompound,
	ArrayInstancePropertySize size, ArrayInstancePropertyGet get, ArrayInstancePropertySet set) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_propertyCategory = PropertyCategory::array_property;
	m_arraySize = size;
	m_arrayGet = get;
	m_arraySet = set;
}

InstanceProperty::InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound typeCompound,
	CollectionInstancePropertySize size, CollectionInstancePropertyIterate iterate, CollectionInstancePropertyGet get, CollectionInstancePropertySet set) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_propertyCategory = PropertyCategory::collection_property;
	m_collectionIterate = iterate;
	m_collectionGet = get;
	m_collectionSet = set;
}



StaticProperty::StaticProperty(const char* name, Attributes* attributes, Type* type, TypeCompound typeCompound,
	SimpleStaticPropertyGet get, SimpleStaticPropertySet set) :
	Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_propertyCategory = PropertyCategory::simple_property;
	m_simpleGet = get;
	m_simpleSet = set;
}

StaticProperty::StaticProperty(const char* name, Attributes* attributes, Type* type, TypeCompound typeCompound,
	ArrayStaticPropertySize size, ArrayStaticPropertyGet get, ArrayStaticPropertySet set) :
	Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_propertyCategory = PropertyCategory::array_property;
	m_arraySize = size;
	m_arrayGet = get;
	m_arraySet = set;
}

StaticProperty::StaticProperty(const char* name, Attributes* attributes, Type* type, TypeCompound typeCompound,
	CollectionStaticPropertySize size, CollectionStaticPropertyIterate iterate, CollectionStaticPropertyGet get, CollectionStaticPropertySet set) :
	Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_propertyCategory = PropertyCategory::collection_property;
	m_collectionIterate = iterate;
	m_collectionGet = get;
	m_collectionSet = set;
}

END_PAF