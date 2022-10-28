#include "Metadata.h"
#include "Metadata.mh"
#include "Metadata.ic"
#include "Metadata.mc"
#include "Type.h"
#include <string.h>
#include <algorithm>

BEGIN_PAF

class CompareAttributeByName
{
public:
	bool operator()(const Attribute& a1, const Attribute& a2)
	{
		return strcmp(a1.name, a2.name) < 0;
	}
};

Metadata::Metadata(const char* name, Attributes* attributes)
{
	m_name = name;
	m_attributes = attributes;
}

MetaCategory Metadata::get__metaCategory_() const
{
	Type* type = const_cast<Metadata*>(this)->getType();
	return type->getMetaCategory();
}

string_t Metadata::_getAttributeName_(size_t index)
{
	if (m_attributes && index < m_attributes->count)
	{
		return m_attributes->attributes[index].name;
	}
	return "";
}

string_t Metadata::_getAttributeContent_(size_t index)
{
	if (m_attributes && index < m_attributes->count)
	{
		return m_attributes->attributes[index].content;
	}
	return "";
}

string_t Metadata::_getAttributeContentByName_(string_t attributeName)
{
	if (0 != m_attributes)
	{
		Attribute* begin = &m_attributes->attributes[0];
		Attribute* end = begin + m_attributes->count;
		Attribute dummy;
		dummy.name = attributeName;
		Attribute* it = std::lower_bound(begin, end, dummy, CompareAttributeByName());
		if (end != it && strcmp(attributeName, it->name) == 0)
		{
			return it->content;
		}
	}
	return "";
}

bool Metadata::_hasAttribute_(string_t attributeName)
{
	if (0 != m_attributes)
	{
		Attribute* begin = &m_attributes->attributes[0];
		Attribute* end = begin + m_attributes->count;
		Attribute dummy;
		dummy.name = attributeName;
		Attribute* it = std::lower_bound(begin, end, dummy, CompareAttributeByName());
		if (end != it && strcmp(attributeName, it->name) == 0)
		{
			return true;
		}
	}
	return false;
}

bool Metadata::operator < (const Metadata& arg) const
{
	return strcmp(m_name, arg.m_name) < 0;
}

bool CompareMetaDataPtrByName::operator()(const Metadata* m1, const Metadata* m2)
{
	return strcmp(m1->m_name, m2->m_name) < 0;
}

END_PAF
