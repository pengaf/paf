#include "Metadata.h"
#include "Metadata.mh"
#include "Metadata.ic"
#include "Metadata.mc"
#include "Type.h"
#include <string.h>
#include <algorithm>

BEGIN_PAFCORE

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

long_t Metadata::addRef()
{
	return 0x40000000;
};

long_t Metadata::release()
{
	return 0x40000000;
};

MetaCategory Metadata::get__category_() const
{
	Type* type = const_cast<Metadata*>(this)->getType();
	return type->getCategory();
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

const char* g_errorStrings[] =
{
	"s_ok",
	"e_invalid_namespace",
	"e_name_conflict",
	"e_void_variant",
	"e_is_not_type",
	"e_is_not_class",
	"e_invalid_subscript_type",
	"e_member_not_found",
	"e_index_out_of_range",
	"e_is_not_property",
	"e_is_not_array_property",
	"e_is_not_collection_property",
	"e_property_get_not_implemented",
	"e_property_set_not_implemented",
	"e_property_size_not_implemented",
	"e_property_iterate_not_implemented",
	"e_field_is_an_array",
	"e_invalid_type",
	"e_invalid_object_type",
	"e_invalid_field_type",
	"e_invalid_property_type",
	"e_invalid_too_few_arguments",
	"e_invalid_too_many_arguments",
	"e_invalid_this_type",
	"e_invalid_arg_type_1",
	"e_invalid_arg_type_2",
	"e_invalid_arg_type_3",
	"e_invalid_arg_type_4",
	"e_invalid_arg_type_5",
	"e_invalid_arg_type_6",
	"e_invalid_arg_type_7",
	"e_invalid_arg_type_8",
	"e_invalid_arg_type_9",
	"e_invalid_arg_type_10",
	"e_invalid_arg_type_11",
	"e_invalid_arg_type_12",
	"e_invalid_arg_type_13",
	"e_invalid_arg_type_14",
	"e_invalid_arg_type_15",
	"e_invalid_arg_type_16",
	"e_invalid_arg_type_17",
	"e_invalid_arg_type_18",
	"e_invalid_arg_type_19",
	"e_invalid_arg_type_20",
	"e_invalid_arg_type_21",
	"e_invalid_arg_type_22",
	"e_invalid_arg_type_23",
	"e_invalid_arg_type_24",
	"e_invalid_arg_type_25",
	"e_invalid_arg_type_26",
	"e_invalid_arg_type_27",
	"e_invalid_arg_type_28",
	"e_invalid_arg_type_29",
	"e_invalid_arg_type_30",
	"e_invalid_arg_type_31",
	"e_invalid_arg_type_32",
	"e_not_implemented",
	"e_script_error",
	"e_script_dose_not_override",
};


const char* ErrorCodeToString(ErrorCode errorCode)
{
	if (size_t(errorCode) < paf_array_size_of(g_errorStrings))
	{
		return g_errorStrings[size_t(errorCode)];
	}
	return "unknown error";
}

END_PAFCORE
