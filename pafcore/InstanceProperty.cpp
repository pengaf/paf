#include "InstanceProperty.h"
#include "InstanceProperty.mh"
#include "InstanceProperty.ic"
#include "InstanceProperty.mc"

BEGIN_PAFCORE

InstanceProperty::InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
	SimpleInstancePropertyGet get, SimpleInstancePropertySet set) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_getterTypeCompound = getterTypeCompound;
	m_setterTypeCompound = setterTypeCompound;
	m_propertyCategory = PropertyCategory::simple_property;
	m_simpleGet = get;
	m_simpleSet = set;
}

InstanceProperty::InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
	ArrayInstancePropertySize size, ArrayInstancePropertyGet get, ArrayInstancePropertySet set) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_getterTypeCompound = getterTypeCompound;
	m_setterTypeCompound = setterTypeCompound;
	m_propertyCategory = PropertyCategory::array_property;
	m_arraySize = size;
	m_arrayGet = get;
	m_arraySet = set;
}

InstanceProperty::InstanceProperty(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound getterTypeCompound, TypeCompound setterTypeCompound,
	CollectionInstancePropertyIterate iterate, CollectionInstancePropertyGet get, CollectionInstancePropertySet set) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_getterTypeCompound = getterTypeCompound;
	m_setterTypeCompound = setterTypeCompound;
	m_propertyCategory = PropertyCategory::collection_property;
	m_collectionIterate = iterate;
	m_collectionGet = get;
	m_collectionSet = set;
}

END_PAFCORE