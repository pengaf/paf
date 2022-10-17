#pragma once

#include "Variant.h"
#include "PrimitiveType.h"
#include "EnumType.h"
#include "ClassType.h"
#include "Introspectable.h"
#include "Debug.h"
#include <assert.h>
#include <memory>

BEGIN_PAFCORE

//
//#ifdef _DEBUG
//
//class VariantLeakReporter
//{
//public:
//	~VariantLeakReporter()
//	{
//		m_liveObjects.lock();
//		for (auto& item : m_liveObjects.m_objects)
//		{
//			Variant* variant = item.first;
//			size_t serialNumber = item.second;
//			char buf[1024];
//#ifdef _WIN64
//			sprintf_s(buf, "pafcore Warning: Live Variant at 0x%p, SerialNumber:%llu\n",
//				variant, serialNumber);
//#else
//			sprintf_s(buf, "pafcore Warning: Live Variant at 0x%p, SerialNumber:%lu\n",
//				variant, serialNumber);
//#endif
//			OutputDebugStringA(buf);
//		}
//		m_liveObjects.unlock();
//	}
//public:
//	void onVariantConstruct(Variant* variant)
//	{
//		m_liveObjects.addPtr(variant);
//	}
//	void onVariantDestruct(Variant* variant)
//	{
//		m_liveObjects.removePtr(variant);
//	}
//public:
//	LiveObjects<Variant> m_liveObjects;
//public:
//	static VariantLeakReporter* GetInstance()
//	{
//		static VariantLeakReporter s_instance;
//		return &s_instance;
//	}
//};
//
//#endif//_DEBUG
//


Variant::Variant()
{
	m_category = vt_null;
#ifdef _DEBUG
	//VariantLeakReporter::GetInstance()->onVariantConstruct(this);
#endif//_DEBUG
}

Variant::~Variant()
{
	clear();
#ifdef _DEBUG
	//VariantLeakReporter::GetInstance()->onVariantDestruct(this);
#endif//_DEBUG
}

void Variant::clear()
{
	switch (m_category)
	{
	case vt_small_value:
		reinterpret_cast<ValueBox*>(m_storage)->~ValueBox();
		break;
	case vt_big_value:
		delete reinterpret_cast<ValueBox*>(m_ptr);
		break;
	case vt_borrowed_ptr:
		Box::DecBorrowCount(m_ptr);
		break;
	case vt_borrowed_array:
		ArrayBox::DecBorrowCount(m_ptr);
		break;
	case vt_shared_ptr:
		Box::DecOwnerCount(m_ptr);
		break;
	case vt_shared_array:
		ArrayBox::DecOwnerCount(m_ptr, m_arraySize);
		break;
	case vt_unique_ptr:
		Box::DecOwnerCount(m_ptr);
		break;
	case vt_unique_array:
		ArrayBox::DecOwnerCount(m_ptr, m_arraySize);
		break;
	}
	m_category = vt_null;
}

void* Variant::getRawPointer() const
{
	switch (m_category)
	{
	case vt_null:
		return nullptr;
	case vt_small_value:
		return (void*)m_storage;
	default:
		return m_ptr;
	}
}

void Variant::assignEnumByInt(EnumType* type, int value)
{
	clear();
	m_type = type;
	*(int*)m_storage = value;
	m_category = vt_small_value;
}

void Variant::assignRawPtr(Type* type, void* ptr)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_category = vt_raw_ptr;
}

void Variant::assignRawArray(Type* type, void* ptr, size_t size)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_raw_array;
}

void Variant::assignBorrowedPtr(Type* type, void* ptr)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_category = vt_borrowed_ptr;
}

void Variant::assignBorrowedArray(Type* type, void* ptr, size_t size)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_borrowed_array;
}

void Variant::assignSharedPtr(Type* type, void* ptr)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_category = vt_shared_ptr;
}

void Variant::assignSharedArray(Type* type, void* ptr, size_t size)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_shared_array;
}

void Variant::assignUniquePtr(Type* type, void* ptr)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_category = vt_unique_ptr;
}

void Variant::assignUniqueArray(Type* type, void* ptr, size_t size)
{
	clear();
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_unique_array;
}

bool Variant::castToPrimitive(PrimitiveType* dstType, void* dst) const
{
	switch (m_category)
	{
	case vt_small_value:
		if (m_type->isPrimitive())
		{
			PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
			primitiveType->castTo(dst, dstType, &m_storage[sizeof(ValueBox)]);
			return true;
		}
		else if (m_type->isEnumeration())
		{
			EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
			enumType->castToPrimitive(dst, dstType, &m_storage[sizeof(ValueBox)]);
			return true;
		}
		break;
	case vt_big_value:
	case vt_raw_ptr:
	case vt_raw_array:
	case vt_borrowed_ptr:
	case vt_borrowed_array:
	case vt_unique_ptr:
	case vt_unique_array: 
	case vt_shared_ptr:
	case vt_shared_array:
		if (m_type->isPrimitive())
		{
			PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
			primitiveType->castTo(dst, dstType, m_ptr);
			return true;
		}
		else if (m_type->isEnumeration())
		{
			EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
			enumType->castToPrimitive(dst, dstType, m_ptr);
			return true;
		}
		break;
	}
	return false;
}

bool Variant::castToEnum(EnumType* dstType, void* dst) const
{
	switch (m_category)
	{
	case vt_small_value:
		if (m_type->isPrimitive())
		{
			PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
			dstType->castFromPrimitive(dst, primitiveType, &m_storage[sizeof(ValueBox)]);
			return true;
		}
		else if (m_type->isEnumeration())
		{
			EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
			if (dstType == enumType)
			{
				memcpy(dst, &m_storage[sizeof(ValueBox)], dstType->get__size_());
				return true;
			}
		}
		break;
	case vt_big_value:
	case vt_raw_ptr:
	case vt_raw_array:
	case vt_borrowed_ptr:
	case vt_borrowed_array:
	case vt_unique_ptr:
	case vt_unique_array:
	case vt_shared_ptr:
	case vt_shared_array:
		if (m_type->isPrimitive())
		{
			PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
			dstType->castFromPrimitive(dst, primitiveType, m_ptr);
			return true;
		}
		else if (m_type->isEnumeration())
		{
			EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
			if (dstType == enumType)
			{
				memcpy(dst, m_ptr, dstType->get__size_());
				return true;
			}
		}
		break;
	}
	return false;
}

bool Variant::castToRawPointer(Type* dstType, void** dst) const
{
	if(vt_null == m_category)
	{
		return false;
	}
	Type* srcType = m_type;
	const void* src = m_ptr;
	switch (m_category)
	{
	case vt_small_value:
		src = &m_storage[sizeof(ValueBox)];
		break;
	}
	if (srcType == dstType)
	{
		*dst = (void*)src;
		return true;
	}
	if (srcType->isObject() && dstType->isObject())
	{
		if (static_cast<ClassType*>(srcType)->isIntrospectable())
		{
			Introspectable* introspectable = (Introspectable*)src;
			srcType = introspectable->getType();
			src = introspectable->getAddress();
		}
		size_t offset;
		if (static_cast<ClassType*>(srcType)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
		{
			*dst = (void*)((size_t)src + offset);
			return true;
		}
	}
	return false;
}


END_PAFCORE

