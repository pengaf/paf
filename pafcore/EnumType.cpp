#include "EnumType.h"
#include "EnumType.mh"
#include "EnumType.ic"
#include "EnumType.mc"
#include "InstanceProperty.h"
#include <algorithm>

BEGIN_PAFCORE

EnumType::EnumType(const char* name, const char* declarationFile)
: Type(name, enumeration, declarationFile)
{
	m_enumerators = 0;
	m_enumeratorCount = 0;
	static pafcore::InstanceProperty s_properties[] = 
	{
		::pafcore::InstanceProperty("_name_", 0, 0, RuntimeTypeOf<string_t>::RuntimeType::GetSingleton(), false, Enum_get__name_, 0, 0, 0),
	};
	m_instanceProperties = s_properties;
	m_instancePropertyCount = paf_array_size_of(s_properties);
}

size_t EnumType::_getEnumeratorCount_()
{
	return m_enumeratorCount;
}

Enumerator* EnumType::_getEnumerator_(size_t index)
{
	if(index < m_enumeratorCount)
	{
		return &m_enumerators[index];
	}
	return 0;
}

Enumerator* EnumType::_getEnumeratorByValue_(int value)
{
	for(size_t i = 0; i < m_enumeratorCount; ++i)
	{
		Enumerator* enumerator = &m_enumerators[i];
		if(value == enumerator->m_value)
		{
			return enumerator;
		}
	}
	return 0;
}

Enumerator* EnumType::_getEnumeratorByName_(string_t name)
{
	Metadata dummy(name);
	Enumerator* res = std::lower_bound(m_enumerators, m_enumerators + m_enumeratorCount, dummy);
	if(m_enumerators + m_enumeratorCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return 0;
}

Enumerator* EnumType::findEnumerator(const char* name)
{
	Metadata dummy(name);
	Enumerator* res = std::lower_bound(m_enumerators, m_enumerators + m_enumeratorCount, dummy);
	if(m_enumerators + m_enumeratorCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return 0;
}

pafcore::ErrorCode EnumType::Enum_get__name_(pafcore::InstanceProperty* instanceProperty, pafcore::Variant* that, pafcore::Variant* value)
{
	if(enumeration != that->m_type->m_category)
	{
		return pafcore::e_invalid_this_type;
	}
	int e;
	if(!that->castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &e))
	{
		return pafcore::e_invalid_this_type;
	}
	EnumType* enumType = static_cast<EnumType*>(that->m_type);
	Enumerator* enumerator = enumType->_getEnumeratorByValue_(e);
	if(0 == enumerator)
	{	
		return pafcore::e_invalid_this_type;
	}
	string_t res = enumerator->get__name_();
	value->assignPrimitive(RuntimeTypeOf<string_t>::RuntimeType::GetSingleton(), &res);
	return pafcore::s_ok;
}

Metadata* EnumType::findMember(const char* name)
{
	Metadata dummy(name);
	InstanceProperty* res = std::lower_bound(m_instanceProperties, m_instanceProperties + m_instancePropertyCount, dummy);
	if (m_instanceProperties + m_instancePropertyCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return 0;
}

void EnumType::castToPrimitive(void* dst, PrimitiveType* dstType, const void* src)
{
	switch (m_size)
	{
	case 1:
		CharType::GetSingleton()->castTo(dst, dstType, src);
		break;
	case 2:
		ShortType::GetSingleton()->castTo(dst, dstType, src);
		break;
	case 4:
		IntType::GetSingleton()->castTo(dst, dstType, src);
		break;
	case 8:
		LongLongType::GetSingleton()->castTo(dst, dstType, src);
		break;
	default:
		PAF_ASSERT(false);
	}
}

void EnumType::castFromPrimitive(void* dst, PrimitiveType* srcType, const void* src)
{
	switch (m_size)
	{
	case 1:
		srcType->castTo(dst, CharType::GetSingleton(), src);
		break;
	case 2:
		srcType->castTo(dst, ShortType::GetSingleton(), src);
		break;
	case 4:
		srcType->castTo(dst, IntType::GetSingleton(), src);
		break;
	case 8:
		srcType->castTo(dst, LongLongType::GetSingleton(), src);
		break;
	default:
		PAF_ASSERT(false);
	}
}

END_PAFCORE
