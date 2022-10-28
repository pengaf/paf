#include "Field.h"
#include "Field.mh"
#include "Field.ic"
#include "Field.mc"

BEGIN_PAF

InstanceField::InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound typeCompound, uint32_t arraySize, uint32_t offset) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_arraySize = arraySize;
	m_offset = offset;
}

StaticField::StaticField(const char* name, Attributes* attributes, Type* type, TypeCompound typeCompound, uint32_t arraySize, size_t address) :
	Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_arraySize = arraySize;
	m_address = address;
}

END_PAF
