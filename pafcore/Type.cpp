#include "Type.h"
#include "Type.mh"
#include "Type.ic"
#include "Type.mc"
#include "NameSpace.h"

BEGIN_PAF

Type::Type(const char* name, MetaCategory metaCategory, const char* declarationFile) :
	Metadata(name),
	m_metaCategory(metaCategory),
	m_size(0),
	m_enclosing(0),
	m_declarationFile(declarationFile)
{
}

Type::~Type()
{
	if (m_enclosing && MetaCategory::name_space == m_enclosing->getType()->getMetaCategory())
	{
		static_cast<NameSpace*>(m_enclosing)->unregisterMember(this);
	}
}

::paf::ErrorCode Type::placementNew(void* address, ::paf::Variant** args, uint32_t numArgs)
{
	return ::paf::ErrorCode::e_not_implemented;
}

bool Type::placementNewArray(void* address, size_t count)
{
	return false;
}

bool Type::destruct(void* address)
{
	return false;
}

bool Type::copyConstruct(void* dst, const void* src)
{
	return false;
}

bool Type::copyAssign(void* dst, const void* src)
{
	return false;
}

Metadata* Type::findMember(const char* name)
{
	return nullptr;
}


END_PAF