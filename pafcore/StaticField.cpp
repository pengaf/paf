#include "StaticField.h"
#include "StaticField.mh"
#include "StaticField.ic"
#include "StaticField.mc"

BEGIN_PAFCORE

StaticField::StaticField(const char* name, Attributes* attributes, Type* type, TypeCompound typeCompound, uint32_t arraySize, size_t address) :
	Metadata(name, attributes)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_arraySize = arraySize;
	m_address = address;
}


END_PAFCORE
