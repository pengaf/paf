#include "Argument.h"
#include "Argument.mh"
#include "Argument.ic"
#include "Argument.mc"

BEGIN_PAFCORE

Argument::Argument(const char* name, Type* type, TypeCompound typeCompound, Passing passing) :
	Metadata(name)
{
	m_type = type;
	m_typeCompound = typeCompound;
	m_passing = passing;
}


END_PAFCORE