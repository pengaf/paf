#include "Enumerator.h"
#include "Enumerator.mh"
#include "Enumerator.ic"
#include "Enumerator.mc"

BEGIN_PAFCORE

Enumerator::Enumerator(const char* name, Attributes* attributes, EnumType* type, int value)
: Metadata(name, attributes)
{
	m_type = type;
	m_value = value;
}


END_PAFCORE
