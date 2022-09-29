#pragma once

#include "Variant.h"
#include "VoidType.h"
#include "PrimitiveType.h"
#include "EnumType.h"
#include "ClassType.h"
#include "Reference.h"
#include "Debug.h"
#include <assert.h>
#include <memory>

BEGIN_PAFCORE


#ifdef _DEBUG

class VariantLeakReporter
{
public:
	~VariantLeakReporter()
	{
		m_liveObjects.lock();
		for (auto& item : m_liveObjects.m_objects)
		{
			Variant* variant = item.first;
			size_t serialNumber = item.second;
			char buf[1024];
#ifdef _WIN64
			sprintf_s(buf, "pafcore Warning: Live Variant at 0x%p, SerialNumber:%llu\n",
				variant, serialNumber);
#else
			sprintf_s(buf, "pafcore Warning: Live Variant at 0x%p, SerialNumber:%lu\n",
				variant, serialNumber);
#endif
			OutputDebugStringA(buf);
		}
		m_liveObjects.unlock();
	}
public:
	void onVariantConstruct(Variant* variant)
	{
		m_liveObjects.addPtr(variant);
	}
	void onVariantDestruct(Variant* variant)
	{
		m_liveObjects.removePtr(variant);
	}
public:
	LiveObjects<Variant> m_liveObjects;
public:
	static VariantLeakReporter* GetInstance()
	{
		static VariantLeakReporter s_instance;
		return &s_instance;
	}
};

#endif//_DEBUG



Variant::Variant()
{
	m_category = vt_null;
#ifdef _DEBUG
	VariantLeakReporter::GetInstance()->onVariantConstruct(this);
#endif//_DEBUG
}

Variant::~Variant()
{
	clear();
#ifdef _DEBUG
	VariantLeakReporter::GetInstance()->onVariantDestruct(this);
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
		ArrayBox::DecOwnerCount(m_ptr, m_size);
		break;
	case vt_unique_ptr:
		Box::DecOwnerCount(m_ptr);
		break;
	case vt_unique_array:
		ArrayBox::DecOwnerCount(m_ptr, m_size);
		break;
	}
	m_category = vt_null;
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
	m_size = size;
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
	m_size = size;
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
	m_size = size;
	m_category = vt_unique_array;
}

//void Variant::assignObject(Type* type, const void* pointer, bool constant, bool hold)
//{
//	switch(type->m_category)
//	{
//	case primitive:
//		assignPrimitive(type, pointer);
//		break;
//	case enumeration:
//		assignEnum(type, pointer);
//		break;
//	case class_object:
//		assignValuePtr(type, pointer, constant, hold);
//		break;
//	default:
//		assignReferencePtr(type, pointer, constant, hold);
//	}
//}
//

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
		else if (m_type->isEnum())
		{
			EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
			enumType->castToPrimitive(dst, dstType, &m_storage[sizeof(ValueBox)]);
			return true;
		}
		break;
	case vt_big_value:
	case vt_borrowed_ptr:
	case vt_borrowed_array:
	case vt_shared_ptr:
	case vt_shared_array:
	case vt_unique_ptr:
	case vt_unique_array: 
		if (m_type->isPrimitive())
		{
			PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
			primitiveType->castTo(dst, dstType, m_ptr);
			return true;
		}
		else if (m_type->isEnum())
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
		else if (m_type->isEnum())
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
	case vt_borrowed_ptr:
	case vt_borrowed_array:
	case vt_shared_ptr:
	case vt_shared_array:
	case vt_unique_ptr:
	case vt_unique_array:
		if (m_type->isPrimitive())
		{
			PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
			dstType->castFromPrimitive(dst, primitiveType, m_ptr
			return true;
		}
		else if (m_type->isEnum())
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

//bool Variant::castToValuePtr(Type* dstType, void** dst) const
//{
//	assert(dstType->isObject());
//	if (0 == m_pointer)
//	{
//		return false;
//	}
//	size_t offset;
//	if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
//	{
//		*dst = (void*)((size_t)m_pointer + offset);
//		return true;
//	}
//	return false;
//}

bool Variant::castToRawPtr(Type* dstType, void** dst) const
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
	if (srcType->isReference())
	{
		Object* object = (Object*)src;
		srcType = object->getType();
		src = (void*)object->getAddress();
	}
	size_t offset;
	if (static_cast<ClassType*>(srcType)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
	{
		*dst = (void*)((size_t)src + offset);
		return true;
	}
	return false;
}

//
//bool Variant::castToEnum(Type* dstType, void* dst) const
//{
//	assert(dstType->isEnum());
//	int tmp;
//	if(m_type->isPrimitive())
//	{
//		static_cast<PrimitiveType*>(m_type)->castTo(&tmp, IntType::GetSingleton(), m_pointer);
//	}
//	else if(m_type->isEnum())
//	{
//		switch (m_type->m_size)
//		{
//		case 1:
//			CharType::GetSingleton()->castTo(&tmp, IntType::GetSingleton(), m_pointer);
//			break;
//		case 2:
//			ShortType::GetSingleton()->castTo(&tmp, IntType::GetSingleton(), m_pointer);
//			break;
//		case 4:
//			IntType::GetSingleton()->castTo(&tmp, IntType::GetSingleton(), m_pointer);
//			break;
//		default:
//			assert(false);
//		}
//	}
//	else
//	{
//		return false;
//	}
//	switch (dstType->m_size)
//	{
//	case 1:
//		IntType::GetSingleton()->castTo(dst, CharType::GetSingleton(), &tmp);
//		return true;
//	case 2:
//		IntType::GetSingleton()->castTo(dst, ShortType::GetSingleton(), &tmp);
//		return true;
//	case 4:
//		IntType::GetSingleton()->castTo(dst, IntType::GetSingleton(), &tmp);
//		return true;
//	default:
//		assert(false);
//	}
//	return false;
//}
//
//bool Variant::castToValue(Type* dstType, void* dst) const
//{
//	assert(dstType->isObject());
//	void* ptr;
//	if(castToValuePtr(dstType, &ptr) && ptr)
//	{
//		return dstType->assign(dst, ptr);
//	}
//	return false;
//}
//
//bool Variant::castToReference(Type* dstType, void* dst) const
//{
//	assert(dstType->isIntrospectionObject());
//	void* ptr;
//	if(castToReferencePtr(dstType, &ptr) && ptr)
//	{
//		return dstType->assign(dst, ptr);
//	}
//	return false;
//}
//
//bool Variant::castToVoidPtr(void** dst) const
//{
//	if (0 != m_pointer)
//	{
//		*dst = m_pointer;
//		return true;
//	}
//	return false;
//}
//
//bool Variant::castToPrimitivePtr(Type* dstType, void** dst) const
//{
//	assert(dstType->isPrimitive());
//	if(m_type == dstType && 0 != m_pointer)
//	{
//		*dst = m_pointer;
//		return true;
//	}
//	return false;
//}
//
//bool Variant::castToEnumPtr(Type* dstType, void** dst) const
//{
//	assert(dstType->isEnum());
//	if(m_type == dstType && 0 != m_pointer)
//	{
//		*dst = m_pointer;
//		return true;
//	}
//	return false;
//}
//
//bool Variant::castToValuePtr(Type* dstType, void** dst) const
//{
//	assert(dstType->isObject());
//	if(0 == m_pointer)
//	{
//		return false;
//	}
//	size_t offset;
//	if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
//	{
//		*dst = (void*)((size_t)m_pointer + offset);
//		return true;
//	}
//	return false;
//}
//
//bool Variant::castToReferencePtr(Type* dstType, void** dst) const
//{
//	assert(dstType->isIntrospectionObject());
//	if(0 == m_pointer)
//	{
//		return false;
//	}
//	if(m_type->isIntrospectionObject())
//	{
//		size_t offset;
//		if(static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
//		{
//			*(size_t*)dst = (size_t)m_pointer + offset;
//			return true;
//		}
//	}
//	return false;
//}
//
//
//bool Variant::castToVoidPtrAllowNull(void** dst) const
//{
//	*dst = m_pointer;
//	return true;
//}
//
//bool Variant::castToPrimitivePtrAllowNull(Type* dstType, void** dst) const
//{
//	if (0 == m_pointer)
//	{
//		*dst = 0;
//		return true;
//	}
//	assert(dstType->isPrimitive());
//	if (m_type == dstType)
//	{
//		*dst = m_pointer;
//		return true;
//	}
//	return false;
//}
//
//bool Variant::castToEnumPtrAllowNull(Type* dstType, void** dst) const
//{
//	if (0 == m_pointer)
//	{
//		*dst = 0;
//		return true;
//	}
//	assert(dstType->isEnum());
//	if (m_type == dstType)
//	{
//		*dst = m_pointer;
//		return true;
//	}
//	return false;
//}
//
//bool Variant::castToValuePtrAllowNull(Type* dstType, void** dst) const
//{
//	if (0 == m_pointer)
//	{
//		*dst = 0;
//		return true;
//	}
//	assert(dstType->isObject());
//	if (m_type->isObject())
//	{
//		size_t offset;
//		if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
//		{
//			*dst = (void*)((size_t)m_pointer + offset);
//			return true;
//		}
//	}
//	return false;
//}
//
//bool Variant::castToReferencePtrAllowNull(Type* dstType, void** dst) const
//{
//	if (0 == m_pointer)
//	{
//		*dst = 0;
//		return true;
//	}
//	assert(dstType->isIntrospectionObject());
//	if (m_type->isIntrospectionObject())
//	{
//		size_t offset;
//		if (static_cast<ClassType*>(m_type)->getClassOffset(offset, static_cast<ClassType*>(dstType)))
//		{
//			*(size_t*)dst = (size_t)m_pointer + offset;
//			return true;
//		}
//	}
//	return false;
//}
//
//bool Variant::castToObjectPtr(Type* dstType, void** dst) const
//{
//	switch (dstType->m_category)
//	{
//	case void_object:
//		return castToVoidPtr(dst);
//	case primitive:
//		return castToPrimitivePtr(dstType, dst);
//	case enumeration:
//		return castToEnumPtr(dstType, dst);
//	case class_object:
//		return castToValuePtr(dstType, dst);
//	case introspection_class_object:
//		return castToReferencePtr(dstType, dst);
//	}
//	return false;
//}
//
//bool Variant::castToObject(Type* dstType, void* dst) const
//{
//	switch(dstType->m_category)
//	{
//	case primitive:
//		return castToPrimitive(dstType, dst);
//	case enumeration:
//		return castToEnum(dstType, dst);
//	case class_object:
//		return castToValue(dstType, dst);
//	case introspection_class_object:
//		return castToReference(dstType, dst);
//	}
//	return false;
//}
//
//void Variant::reinterpretCastToPtr(Variant& var, Type* dstType) const
//{
//	assert(var.isNull() && 0 == var.m_arraySize);
//	assert(dstType && dstType->m_size);
//	var.m_type = dstType;
//	var.m_pointer = m_pointer;
//	var.m_constant = m_constant;
//	size_t size = m_type->m_size *(0 == m_arraySize ? 1 : m_arraySize);
//	var.m_arraySize = size / dstType->m_size;
//	var.m_semantic = by_ptr;
//}

END_PAFCORE

