#pragma once

#include "Variant.h"
#include "PrimitiveType.h"
#include "EnumType.h"
#include "ClassType.h"

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

inline bool IsFinal(Type* type, void* ptr)
{
	if (int(type->getMetaCategory()) >= int(MetaCategory::object))
	{
		ClassType* classType = static_cast<ClassType*>(type);
		if (classType->isIntrospectable())
		{
			Type* finalType = reinterpret_cast<Introspectable*>(ptr)->getType();
			return finalType == type;
		}
	}
	return true;
}

inline void DynamicCast(Type*& type, void*& ptr)
{
	if (int(type->getMetaCategory()) >= int(MetaCategory::object))
	{
		ClassType* classType = static_cast<ClassType*>(type);
		if (classType->isIntrospectable())
		{
			type = reinterpret_cast<Introspectable*>(ptr)->getType();
			ptr = reinterpret_cast<Introspectable*>(ptr)->getAddress();
		}
	}
}

Variant::Variant()
{
	m_category = vt_null;
#ifdef _DEBUG
	//VariantLeakReporter::GetInstance()->onVariantConstruct(this);
#endif//_DEBUG
}

Variant::Variant(Variant&& var)
{
	m_category = var.m_category;
	m_arrayIndex = var.m_arrayIndex;
	m_type = var.m_type;
	if (vt_small_value == var.m_category)
	{
		memcpy(m_storage, var.m_storage, storage_size);
	}
	else
	{
		m_ptr = var.m_ptr;
		m_arraySize = var.m_arraySize;
	}
	var.m_category = vt_null;
}

Variant::~Variant()
{
	clear();
#ifdef _DEBUG
	//VariantLeakReporter::GetInstance()->onVariantDestruct(this);
#endif//_DEBUG
}

void Variant::assign(Variant&& var)
{
	clear();
	m_category = var.m_category;
	m_arrayIndex = var.m_arrayIndex;
	m_type = var.m_type;
	if (vt_small_value == var.m_category)
	{
		memcpy(m_storage, var.m_storage, storage_size);
	}
	else
	{
		m_ptr = var.m_ptr;
		m_arraySize = var.m_arraySize;
	}
	var.m_category = vt_null;
}

void Variant::clear()
{
	switch (m_category)
	{
	case vt_small_value:
		reinterpret_cast<ValueBox*>(m_storage)->~ValueBox();
		break;
	case vt_big_value:
		ValueBox::Delete(m_ptr);
		break;
	case vt_borrowed_ptr:
		Box::DecBorrowCount(m_ptr);
		break;
	case vt_borrowed_array:
		ArrayBox::DecBorrowCount(m_ptr);
		break;
	case vt_unique_ptr:
		Box::DecOwnerCount(m_ptr);
		break;
	case vt_unique_array:
		ArrayBox::DecOwnerCount(m_ptr, m_arraySize);
		break;
	case vt_shared_ptr:
		Box::DecOwnerCount(m_ptr);
		break;
	case vt_shared_array:
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
		return (void*)&m_storage[sizeof(ValueBox)];
	default:
		return m_ptr;
	}
}

void Variant::assignEnumByInt(EnumType* type, int value)
{
	clear();
	m_type = type;
	new (m_storage)ValueBox();
	*(int*)&m_storage[sizeof(ValueBox)] = value;
	m_category = vt_small_value;
}

void Variant::assignRawPtr(Type* type, void* ptr)
{
	clear();
	DynamicCast(type, ptr);
	m_type = type;
	m_ptr = ptr;
	m_category = vt_raw_ptr;
}

void Variant::assignRawArray(Type* type, void* ptr, size_t size)
{
	clear();
	PAF_ASSERT(IsFinal(type, ptr));
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_raw_array;
}

void Variant::assignBorrowedPtr(Type* type, void* ptr)
{
	clear();
	DynamicCast(type, ptr);
	m_type = type;
	m_ptr = ptr;
	m_category = vt_borrowed_ptr;
}

void Variant::assignBorrowedArray(Type* type, void* ptr, size_t size)
{
	clear();
	PAF_ASSERT(IsFinal(type, ptr));
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_borrowed_array;
}

void Variant::assignSharedPtr(Type* type, void* ptr)
{
	clear();
	DynamicCast(type, ptr);
	m_type = type;
	m_ptr = ptr;
	m_category = vt_shared_ptr;
}

void Variant::assignSharedArray(Type* type, void* ptr, size_t size)
{
	clear();
	PAF_ASSERT(IsFinal(type, ptr));
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_shared_array;
}

void Variant::assignUniquePtr(Type* type, void* ptr)
{
	clear();
	DynamicCast(type, ptr);
	m_type = type;
	m_ptr = ptr;
	m_category = vt_unique_ptr;
}

void Variant::assignUniqueArray(Type* type, void* ptr, size_t size)
{
	clear();
	PAF_ASSERT(IsFinal(type, ptr));
	m_type = type;
	m_ptr = ptr;
	m_arraySize = size;
	m_category = vt_unique_array;
}

bool Variant::castToPrimitive(PrimitiveType* dstType, void* dst) const
{
	if (vt_null == m_category)
	{
		return false;
	}
	if (m_type->isPrimitive())
	{
		PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
		primitiveType->castTo(dst, dstType, getRawPointer());
		return true;
	}
	else if (m_type->isEnumeration())
	{
		EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
		enumType->castToPrimitive(dst, dstType, getRawPointer());
		return true;
	}
}

bool Variant::castToEnum(EnumType* dstType, void* dst) const
{
	if (vt_null == m_category)
	{
		return false;
	}
	if (m_type->isPrimitive())
	{
		PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
		dstType->castFromPrimitive(dst, primitiveType, getRawPointer());
		return true;
	}
	else if (m_type->isEnumeration())
	{
		EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
		if (dstType == enumType)
		{
			memcpy(dst, getRawPointer(), dstType->get__size_());
			return true;
		}
	}
	return false;
}

bool Variant::castToString(string_t& str)
{
	if (vt_null == m_category)
	{
		return false;
	}
	if (m_type->isString())
	{ 
		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
		if (paf::string_t::GetType() == classType)
		{
			str = *static_cast<paf::string_t*>(getRawPointer());
			return true;
		}
	}
	else if (m_type->isObject())
	{
		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
		size_t offset;
		if (classType->getClassOffset(offset, paf::StringBase::GetType()))
		{
			str = reinterpret_cast<paf::StringBase*>((size_t)getRawPointer() + offset)->toString();
			return true;
		}
	}
	return false;
}

bool Variant::castToBuffer(buffer_t& buf)
{
	if (vt_null == m_category)
	{
		return false;
	}
	if (m_type->isBuffer())
	{
		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
		if (paf::buffer_t::GetType() == classType)
		{
			buf = *static_cast<paf::buffer_t*>(getRawPointer());
			return true;
		}
	}
	else if (m_type->isObject())
	{
		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
		size_t offset;
		if (classType->getClassOffset(offset, paf::BufferBase::GetType()))
		{
			buf = reinterpret_cast<paf::BufferBase*>((size_t)getRawPointer() + offset)->toBuffer();
			return true;
		}
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
	if (srcType->isClass() && dstType->isClass())
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

ErrorCode Variant::subscript(Variant& var, uint32_t index)
{
	var.clear();
	switch (m_category)
	{
	case vt_raw_array:
	case vt_borrowed_array:
	case vt_unique_array:
	case vt_shared_array:
		if (index < m_arraySize)
		{
			var.m_type = m_type;
			var.m_ptr = (void*)((size_t)m_ptr + m_type->get__size_() * index);
			var.m_category = vt_reference;
			return ErrorCode::s_ok;
		}
		else
		{
			return ErrorCode::e_index_out_of_range;
		}
	case vt_raw_ptr:
	case vt_borrowed_ptr:
	case vt_unique_ptr:
	case vt_shared_ptr:
		if (0 == index)
		{
			var.m_type = m_type;
			var.m_ptr = m_ptr;
			var.m_category = vt_reference;
			return ErrorCode::s_ok;
		}
		else
		{
			return ErrorCode::e_index_out_of_range;
		}
	}
	return ErrorCode::e_is_not_array;
}

END_PAFCORE

