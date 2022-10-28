#include "TypeAlias.h"
#include "TypeAlias.mh"
#include "TypeAlias.ic"
#include "TypeAlias.mc"
#include "NameSpace.h"

BEGIN_PAF

TypeAlias::TypeAlias(const char* name, Type* type, const char* declarationFile) :
	Metadata(name),
	m_type(type),
	m_enclosing(0),
	m_declarationFile(declarationFile)
{
}

TypeAlias::~TypeAlias()
{
	if (m_enclosing && MetaCategory::name_space == m_enclosing->getType()->getMetaCategory())
	{
		static_cast<NameSpace*>(m_enclosing)->unregisterMember(this);
	}
}


::paf::RawPtr<Type> TypeAlias::get_type() const
{
	return m_type;
}


END_PAF
