#include "Result.h"
#include "Result.mh"
#include "Result.ic"
#include "Result.mc"

BEGIN_PAFCORE

Result::Result(Type* type, TypeCompound typeCompound)
: Metadata(0)
{
	m_type = type;
	m_typeCompound = typeCompound;
}

Type* Result::get_type() const
{
	return m_type;
}

TypeCompound Result::get_typeCompound() const
{
	return m_typeCompound;
}

END_PAFCORE