#pragma once

#include "Variant.h"
#include "PrimitiveType.h"
#include "EnumType.h"
#include "ClassType.h"
#include "Utility.mh"

BEGIN_PAF


class GenericRefCountImpl : public RefCountBase
{
	GenericRefCountImpl() = default;
public:
	void destruct() override
	{
		Type* type = *(reinterpret_cast<Type**>(this) - 1);
		if (type->isObject())
		{
			ClassType* classType = static_cast<ClassType*>(type);
			if (!classType->is_trivially_destructible())
			{
				classType->destruct(this + 1);
			}
		}
	}
	void deallocate() override
	{
		free(reinterpret_cast<Type**>(this) - 1);
	}
public:
	static ErrorCode New(void*& address, Type* type, Variant** args, uint32_t numArgs)
	{
		size_t size = sizeof(Type*) + sizeof(GenericRefCountImpl) + type->get__size_();
		void* p = malloc(size);
		Type** ppType = reinterpret_cast<Type**>(p);
		GenericRefCountImpl* box = reinterpret_cast<GenericRefCountImpl*>(ppType + 1);
		void* ptr = (box + 1);
		ErrorCode errorCode = type->construct(ptr, args, numArgs);
		if (ErrorCode::s_ok == errorCode)
		{
			*ppType = type;
			new(box)GenericRefCountImpl();
			address = ptr;
		}
		else
		{
			free(p);
		}
		return errorCode;
	}
};

class GenericArrayBoxImpl : public RefCountBase
{
public:
	void destruct() override
	{
		Type* type = *(reinterpret_cast<Type**>(this) - 1);
		if (type->isObject())
		{
			ClassType* classType = static_cast<ClassType*>(type);
			if (!classType->is_trivially_destructible())
			{
				void* p = this + 1;
				size_t size = classType->get__size_();
				uint32_t arraySize = m_arraySize;
				for (; arraySize > 0; --arraySize)
				{
					classType->destruct(p);
					p = (void*)(size_t(p) + size);
				}
			}
		}
	}
	void deallocate() override
	{
		free(reinterpret_cast<Type**>(this) - 1);
	}
public:
	static ErrorCode NewArray(void*& address, Type* type, size_t arraySize)
	{
		size_t size = sizeof(Type*) + sizeof(GenericArrayBoxImpl) + type->get__size_() * arraySize;
		void* p = malloc(size);
		Type** ppType = reinterpret_cast<Type**>(p);
		GenericArrayBoxImpl* box = reinterpret_cast<GenericArrayBoxImpl*>(ppType + 1);
		void* ptr = (box + 1);
		if (type->constructArray(ptr, arraySize))
		{
			*ppType = type;
			new(box)GenericArrayBoxImpl();
			address = ptr;
			return ErrorCode::s_ok;

		}
		else
		{
			free(p);
			return ErrorCode::e_not_implemented;
		}
	}
	static size_t arraySize(void* address)
	{
		GenericArrayBoxImpl* box = reinterpret_cast<GenericArrayBoxImpl*>(address) - 1;
		return box->m_arraySize;
	}
};

class GenericValueBoxImpl : public ValueBox
{
public:
	void destruct() override
	{
		Type* type = *(reinterpret_cast<Type**>(this) - 1);
		if (type->isObject())
		{
			ClassType* classType = static_cast<ClassType*>(type);
			if (!classType->is_trivially_destructible())
			{
				classType->destruct(this + 1);
			}
		}
	}
	void deallocate() override
	{
		free(reinterpret_cast<Type**>(this) - 1);
	}
public:
	static ErrorCode NewSmall(Type* type, void* storage, Variant** args, uint32_t numArgs)
	{
		static_assert(sizeof(GenericValueBoxImpl) == sizeof(ValueBox), "");
		GenericValueBoxImpl* box = (GenericValueBoxImpl*)storage;
		void* ptr = (box + 1);
		ErrorCode errorCode = type->construct(ptr, args, numArgs);
		if (ErrorCode::s_ok == errorCode)
		{
			new(box)GenericValueBoxImpl();
		}
		return errorCode;
	}
	static ErrorCode NewBig(void*& address, Type* type, Variant** args, uint32_t numArgs)
	{
		static_assert(sizeof(GenericValueBoxImpl) == sizeof(ValueBox), "");
		size_t size = sizeof(Type*) + sizeof(GenericValueBoxImpl) + type->get__size_();
		void* p = malloc(size);
		Type** ppType = reinterpret_cast<Type**>(p);
		GenericValueBoxImpl* box = reinterpret_cast<GenericValueBoxImpl*>(ppType + 1);
		void* ptr = (box + 1);
		ErrorCode errorCode = type->construct(ptr, args, numArgs);
		if (ErrorCode::s_ok == errorCode)
		{
			*ppType = type;
			new(box)GenericValueBoxImpl();
			address = ptr;
		}
		else
		{
			free(p);
		}
		return errorCode;
	}
};

inline bool IsFinal(Type* type, void* ptr)
{
	if (int(type->getMetaCategory()) >= int(MetaCategory::object))
	{
		ClassType* classType = static_cast<ClassType*>(type);
		if (classType->is_introspectable())
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
		if (classType->is_introspectable())
		{
			type = reinterpret_cast<Introspectable*>(ptr)->getType();
			ptr = reinterpret_cast<Introspectable*>(ptr)->getMemoryAddress();
		}
	}
}

Variant::Variant()
{
	static_assert(offsetof(Variant, m_type) + sizeof(Type*) == offsetof(Variant, m_storage));

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
	}
	var.m_category = vt_null;
}

Variant::~Variant()
{
	if (vt_null != m_category)
	{
		clear();
	}
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
	case vt_shared_ptr:
		RefCountBase::DecStrongCount(m_ptr);
		break;
	case vt_shared_array:
		RefCountBase::DecStrongCountForArray(m_ptr);
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

size_t Variant::getArraySize() const
{
	PAF_ASSERT(isArray());
	return GenericArrayBoxImpl::arraySize(m_ptr);
}

void Variant::assignEnumByInt(EnumType* type, int value)
{
	clear();
	m_type = type;
	new (m_storage)ValueBox();
	*(int*)&m_storage[sizeof(ValueBox)] = value;
	m_category = vt_small_value;
}

void Variant::assignReference(Type* type, void* ptr)
{
	clear();
	if (ptr)
	{
		DynamicCast(type, ptr);
		m_type = type;
		m_ptr = ptr;
		m_category = vt_reference;
	}
}

void Variant::assignRawPtr(Type* type, void* ptr)
{
	clear();
	if (ptr)
	{
		DynamicCast(type, ptr);
		m_type = type;
		m_ptr = ptr;
		m_category = vt_raw_ptr;
	}
}

void Variant::assignSharedPtr(Type* type, void* ptr)
{
	clear();
	if (ptr)
	{
		DynamicCast(type, ptr);
		m_type = type;
		m_ptr = ptr;
		m_category = vt_shared_ptr;
	}
}

void Variant::assignSharedArray(Type* type, void* ptr)
{
	clear();
	if (ptr)
	{
		PAF_ASSERT(IsFinal(type, ptr));
		m_type = type;
		m_ptr = ptr;
		m_category = vt_shared_array;
	}
}

bool Variant::castToValue(Type* dstType, void* dst) const
{
	if (isNull())
	{
		return false;
	}
	void* src = getRawPointer();
	if (dstType == m_type)
	{
		return m_type->copyAssign(dst, src);
	}
	else
	{
		void* castedSrc;
		if (castToRawPointer(dstType, &castedSrc))
		{
			return m_type->copyAssign(dst, castedSrc);
		}
	}
	if (m_type->cast(dstType, dst, src))
	{
		return true;
	}
	else if(dstType->assign(dst, m_type, src))
	{
		return true;
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
		if (static_cast<ClassType*>(srcType)->is_introspectable())
		{
			Introspectable* introspectable = (Introspectable*)src;
			srcType = introspectable->getType();
			src = introspectable->getMemoryAddress();
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

bool Variant::castToRawPointerOrValue(Type* dstType, void* dstValue, void** dstPtr) const
{
	if (castToRawPointer(dstType, dstPtr))
	{
		return true;
	}
	if(castToValue(dstType, dstValue))
	{
		*dstPtr = dstValue;
		return true;
	}
	return false;
}

ErrorCode Variant::newValue(Type* type, Variant** args, uint32_t numArgs)
{
	clear();
	ErrorCode errorCode;
	if (sizeof(ValueBox) + type->get__size_() <= sizeof(m_storage))
	{
		errorCode = GenericValueBoxImpl::NewSmall(type, m_storage, args, numArgs);
		if (ErrorCode::s_ok == errorCode)
		{
			m_category = vt_small_value;
			m_type = type;
		}
	}
	else
	{
		errorCode = GenericValueBoxImpl::NewBig(m_ptr, type, args, numArgs);
		if (ErrorCode::s_ok == errorCode)
		{
			m_category = vt_big_value;
			m_type = type;
		}
	}
	return errorCode;
}

ErrorCode Variant::newSharedPtr(Type* type, Variant** args, uint32_t numArgs)
{
	clear();
	ErrorCode errorCode = GenericRefCountImpl::New(m_ptr, type, args, numArgs);
	if (ErrorCode::s_ok == errorCode)
	{
		m_category = vt_shared_ptr;
		m_type = type;
	}
	return errorCode;
}

ErrorCode Variant::newSharedArray(Type* type, size_t count)
{
	clear();
	ErrorCode errorCode = GenericArrayBoxImpl::NewArray(m_ptr, type, count);
	if (ErrorCode::s_ok == errorCode)
	{
		m_category = vt_shared_array;
		m_type = type;
	}
	return errorCode;
}

ErrorCode Variant::subscript(Variant& var, size_t index)
{
	var.clear();
	switch (m_category)
	{
	case vt_shared_array:
		if (index < GenericArrayBoxImpl::arraySize(m_ptr))
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

ErrorCode Variant::newSharedArray(Type* type, Variant** args, uint32_t numArgs)
{
	if (numArgs < 1)
	{
		return ErrorCode::e_too_few_arguments;
	}
	::size_t a0;
	if (!args[0]->castToValue(RuntimeTypeOf<::size_t>::RuntimeType::GetSingleton(), &a0))
	{
		return ErrorCode::e_invalid_arg_type_1;
	}
	if (1 == numArgs)
	{
		return newSharedArray(type, a0);
	}
	return ErrorCode::e_too_many_arguments;
}


//bool Variant::castToPrimitive(PrimitiveType* dstType, void* dst) const
//{
//	if (vt_small_value != m_category && vt_reference != m_category)
//	{
//		return false;
//	}
//	if (m_type->isPrimitive())
//	{
//		PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
//		primitiveType->cast(dstType, dst, getRawPointer());
//		return true;
//	}
//	else if (m_type->isEnumeration())
//	{
//		EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
//		enumType->cast(dstType, dst, getRawPointer());
//		return true;
//	}
//	return false;
//}
//
//bool Variant::castToEnum(EnumType* dstType, void* dst) const
//{
//	if (vt_small_value != m_category && vt_reference != m_category)
//	{
//		return false;
//	}
//	if (m_type->isPrimitive())
//	{
//		PrimitiveType* primitiveType = reinterpret_cast<PrimitiveType*>(m_type);
//		dstType->cast(primitiveType, dst, getRawPointer());
//		return true;
//	}
//	else if (m_type->isEnumeration())
//	{
//		EnumType* enumType = reinterpret_cast<EnumType*>(m_type);
//		if (dstType == enumType)
//		{
//			memcpy(dst, getRawPointer(), dstType->get__size_());
//			return true;
//		}
//	}
//	return false;
//}
//
//bool Variant::castToString(string_t& str)
//{
//	if (!isValue() && !isReference())
//	{
//		return false;
//	}
//	if (m_type->isString())
//	{ 
//		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
//		if (paf::string_t::GetType() == classType)
//		{
//			str = *static_cast<paf::string_t*>(getRawPointer());
//			return true;
//		}
//	}
//	else if (m_type->isObject())
//	{
//		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
//		size_t offset;
//		if (classType->getClassOffset(offset, paf::StringBase::GetType()))
//		{
//			str = reinterpret_cast<paf::StringBase*>((size_t)getRawPointer() + offset)->toString();
//			return true;
//		}
//	}
//	return false;
//}
//
//bool Variant::castToBuffer(buffer_t& buf)
//{
//	if (!isValue() && !isReference())
//	{
//		return false;
//	}
//	if (m_type->isBuffer())
//	{
//		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
//		if (paf::buffer_t::GetType() == classType)
//		{
//			buf = *static_cast<paf::buffer_t*>(getRawPointer());
//			return true;
//		}
//	}
//	else if (m_type->isObject())
//	{
//		paf::ClassType* classType = static_cast<paf::ClassType*>(m_type);
//		size_t offset;
//		if (classType->getClassOffset(offset, paf::BufferBase::GetType()))
//		{
//			buf = reinterpret_cast<paf::BufferBase*>((size_t)getRawPointer() + offset)->toBuffer();
//			return true;
//		}
//	}
//	return false;
//}

bool Variant::castToPrimitive(PrimitiveType* dstType, void* dst) const
{
	return castToValue(dstType, dst);
}

bool Variant::castToEnum(EnumType* dstType, void* dst) const
{
	return castToValue(dstType, dst);
}

bool Variant::castToString(string_t& str)
{
	return castToValue(RuntimeTypeOf<string_t>::RuntimeType::GetSingleton(), &str);
}

bool Variant::castToBuffer(buffer_t& buf)
{
	return castToValue(RuntimeTypeOf<buffer_t>::RuntimeType::GetSingleton(), &buf);
}

END_PAF

