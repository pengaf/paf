#include "Type.h"
#include "Type.mh"
#include "Type.ic"
#include "Type.mc"
#include "NameSpace.h"

BEGIN_PAFCORE

Type::Type(const char* name, MetaCategory category, const char* declarationFile) :
	Metadata(name),
	m_category(category),
	m_size(0),
	m_enclosing(0),
	m_declarationFile(declarationFile)
{
}

Type::~Type()
{
	if (m_enclosing && MetaCategory::name_space == m_enclosing->get__category_())
	{
		static_cast<NameSpace*>(m_enclosing)->unregisterMember(this);
	}
}


END_PAFCORE