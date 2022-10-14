#include "EnumType.h"
#include "EnumType.mh"
#include "EnumType.ic"
#include "EnumType.mc"
#include "InstanceProperty.h"
#include <algorithm>

BEGIN_PAFCORE

EnumType::EnumType(const char* name, const char* declarationFile)
: Type(name, MetaCategory::enumeration, declarationFile)
{
	m_enumerators = 0;
	m_enumeratorCount = 0;
	static pafcore::InstanceProperty s_properties[] = 
	{
		::pafcore::InstanceProperty("_name_", nullptr, nullptr, RuntimeTypeOf<string_t>::RuntimeType::GetSingleton(), ::pafcore::TypeCompound::none, ::pafcore::TypeCompound::none, Enum_get__name_, nullptr),
	};
	m_instanceProperties = s_properties;
	m_instancePropertyCount = paf_array_size_of(s_properties);
}

size_t EnumType::_getEnumeratorCount_()
{
	return m_enumeratorCount;
}

::pafcore::RawPtr<Enumerator> EnumType::_getEnumerator_(size_t index)
{
	if(index < m_enumeratorCount)
	{
		return &m_enumerators[index];
	}
	return nullptr;
}

::pafcore::RawPtr<Enumerator> EnumType::_getEnumeratorByValue_(int value)
{
	for(size_t i = 0; i < m_enumeratorCount; ++i)
	{
		Enumerator* enumerator = &m_enumerators[i];
		if(value == enumerator->m_value)
		{
			return enumerator;
		}
	}
	return nullptr;
}

::pafcore::RawPtr<Enumerator> EnumType::_getEnumeratorByName_(string_t name)
{
	Metadata dummy(name);
	Enumerator* res = std::lower_bound(m_enumerators, m_enumerators + m_enumeratorCount, dummy);
	if(m_enumerators + m_enumeratorCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return nullptr;
}

Enumerator* EnumType::findEnumerator(const char* name)
{
	Metadata dummy(name);
	Enumerator* res = std::lower_bound(m_enumerators, m_enumerators + m_enumeratorCount, dummy);
	if(m_enumerators + m_enumeratorCount != res && strcmp(name, res->m_name) == 0)
	{
		return res;
	}
	return nullptr;
}

pafcore::ErrorCode EnumType::Enum_get__name_(pafcore::InstanceProperty* instanceProperty, pafcore::Variant* that, pafcore::Variant* value)
{
	Type* type = that->getType();
	if(!type->isEnumeration())
	{
		return pafcore::ErrorCode::e_invalid_this_type;
	}
	int e;
	if(!that->castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &e))
	{
		return pafcore::ErrorCode::e_invalid_this_type;
	}
	EnumType* enumType = static_cast<EnumType*>(type);
	Enumerator* enumerator = enumType->_getEnumeratorByValue_(e);
	if(0 == enumerator)
	{	
		return pafcore::ErrorCode::e_invalid_this_type;
	}
	string_t res = enumerator->get__name_();
	value->assignValue<string_t>(res);
	return pafcore::ErrorCode::s_ok;
}

bool EnumType::destruct(void* address)
{
	return true;
}

bool EnumType::copyConstruct(void* dst, const void* src)
{
	switch (m_size)
	{
	case 1:
		*reinterpret_cast<int8_t*>(dst) = *reinterpret_cast<const int8_t*>(src);
		break;
	case 2:
		*reinterpret_cast<int16_t*>(dst) = *reinterpret_cast<const int16_t*>(src);
		break;
	case 4:
		*reinterpret_cast<int32_t*>(dst) = *reinterpret_cast<const int32_t*>(src);
		break;
	case 8:
		*reinterpret_cast<int64_t*>(dst) = *reinterpret_cast<const int64_t*>(src);
		break;
	default:
		PAF_ASSERT(false);
		return false;
	}
	return true;
}

bool EnumType::copyAssign(void* dst, const void* src)
{
	switch (m_size)
	{
	case 1:
		*reinterpret_cast<int8_t*>(dst) = *reinterpret_cast<const int8_t*>(src);
		break;
	case 2:
		*reinterpret_cast<int16_t*>(dst) = *reinterpret_cast<const int16_t*>(src);
		break;
	case 4:
		*reinterpret_cast<int32_t*>(dst) = *reinterpret_cast<const int32_t*>(src);
		break;
	case 8:
		*reinterpret_cast<int64_t*>(dst) = *reinterpret_cast<const int64_t*>(src);
		break;
	default:
		PAF_ASSERT(false);
		return false;
	}
	return true;
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
