#include "InstanceField.h"
#include "InstanceField.mh"
#include "InstanceField.ic"
#include "InstanceField.mc"

BEGIN_PAFCORE

InstanceField::InstanceField(const char* name, Attributes* attributes, ClassType* objectType, Type* type, TypeCompound typeCompound, uint32_t arraySize, uint32_t offset) :
	Metadata(name, attributes)
{
	m_objectType = objectType;
	m_type = type;
	m_typeCompound = typeCompound;
	m_arraySize = arraySize;
	m_offset = offset;
}

END_PAFCORE
