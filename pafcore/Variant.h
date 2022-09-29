#pragma once

#include "Utility.h"
#include "TypeTraits.h"
#include "SmartPtr.h"

BEGIN_PAFCORE

class Type;
class PrimitiveType;
class EnumType;
class Reference;
class Value;


struct ValueBox
{
public:
	virtual ~ValueBox()
	{}
public:
	static void Destruct(void* ptr)
	{
		ValueBox* box = (ValueBox*)(ptr)-1;
		box->~ValueBox();
	}
	static void Delete(void* ptr)
	{
		ValueBox* box = (ValueBox*)(ptr)-1;
		box->~ValueBox();
		free(box);
	}
};

template<typename T>
class ValueBoxImpl : public ValueBox
{
public:
	~ValueBoxImpl()
	{
		T* ptr = reinterpret_cast<T*>(this + 1);
		ptr->~T();
	}
public:
	template<typename... Types>
	static T* New(Types&&... args)
	{
		static_assert(sizeof(ValueBoxImpl) == sizeof(ValueBox), "");
		size_t size = sizeof(ValueBoxImpl) + sizeof(T);
		void* p = malloc(size);
		ValueBoxImpl* box = (ValueBoxImpl*)p;
		new(box)ValueBoxImpl();
		T* ptr = (T*)(box + 1);
		new(ptr)T(std::forward<Types>(args)...);
		return ptr;
	}
	template<typename... Types>
	static T* PlacementNew(void* storage, Types&&... args)
	{
		static_assert(sizeof(ValueBoxImpl) == sizeof(ValueBox), "");
		ValueBoxImpl* box = (ValueBoxImpl*)storage;
		new(box)ValueBoxImpl();
		T* ptr = (T*)(box + 1);
		new(ptr)T(std::forward<Types>(args)...);
		return ptr;
	}
};

class PAFCORE_EXPORT Variant
{
public:
	static constexpr size_t storage_size = 24;//16 + 8
	enum Category
	{
		vt_null,
		vt_small_value,
		vt_big_value,
		vt_unique_ptr,
		vt_unique_array,
		vt_shared_ptr,
		vt_shared_array,
		vt_borrowed_ptr,
		vt_borrowed_array,
		vt_borrowed_array_element,
	};
public:
	Variant();
	~Variant();
private:
	Variant(Variant const& var) = delete;
	Variant& operator = (Variant const& var) = delete;
public:
	bool isNull();

	void clear();
	//void move(Variant& var);
	bool subscript(Variant& var, uint32_t index);

	void assignValue(Type* type, void const* pointer);

	bool castToPrimitive(PrimitiveType* dstType, void* dst) const;
	bool castToEnum(EnumType* dstType, void* dst) const;
	bool castToRawPtr(Type* dstType, void** dst) const;


	//void assignPrimitive(PrimitiveType* type, void const* pointer);

	//void assignEnum(EnumType* enumType, void const* pointer);


	template<typename T>
	void assignBorrowedPtr(BorrowedPtr<T> const& borrowedPtr)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = borrowedPtr.get();
		Box::IncBorrowCount(ptr);
		assignBorrowedPtr(type, ptr);
	}

	template<typename T>
	void assignBorrowedPtr(BorrowedPtr<T>&& borrowedPtr)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = borrowedPtr.get();
		borrowedPtr.m_ptr = nullptr;
		assignBorrowedPtr(type, ptr);
	}

	template<typename T>
	void assignBorrowedArray(BorrowedArray<T> const& borrowedArray)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = borrowedArray.get();
		size_t size = borrowedArray.size();
		ArrayBox::IncBorrowCount(ptr);
		assignBorrowedArray(type, ptr, size);
	}

	template<typename T>
	void assignBorrowedArray(BorrowedArray<T>&& borrowedArray)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = borrowedArray.get();
		size_t size = borrowedArray.size();
		borrowedArray.m_ptr = nullptr;
		borrowedArray.m_size = 0;
		assignBorrowedArray(type, ptr, size);
	}

	template<typename T, typename D>
	void assignSharedPtr(SharedPtr<T, D> const& sharedPtr)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = sharedPtr.get();
		Box::IncOwnerCount(ptr);
		assignSharedPtr(type, ptr);
	}

	template<typename T, typename D>
	void assignSharedPtr(SharedPtr<T, D>&& sharedPtr)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = sharedPtr.get();
		sharedPtr.m_ptr = nullptr;
		assignSharedPtr(type, ptr);
	}

	template<typename T, typename D>
	void assignSharedArray(SharedArray<T, D> const& sharedArray)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = sharedArray.get();
		size_t size = sharedArray.size();
		ArrayBox::IncOwnerCount(ptr);
		assignSharedArray(type, ptr, size);
	}

	template<typename T, typename D>
	void assignSharedArray(SharedArray<T, D> && sharedArray)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = sharedArray.get();
		size_t size = sharedArray.size();
		sharedArray.m_ptr = nullptr;
		sharedArray.m_size = 0;
		assignSharedArray(type, ptr, size);
	}

	template<typename T, typename D>
	void assignUniquePtr(UniquePtr<T, D>&& uniquePtr)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = uniquePtr.get();
		uniquePtr.m_ptr = nullptr;
		assignUniquePtr(type, ptr);
	}

	template<typename T, typename D>
	void assignUniqueArray(UniqueArray<T, D>&& uniqueArray)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = uniqueArray.get();
		size_t size = uniqueArray.size();
		uniquePtr.m_ptr = nullptr;
		uniquePtr.m_size = 0;
		assignUniqueArray(type, ptr, size);
	}

	template<typename T>
	void assignValue(T&& value)
	{
		using T_ = std::remove_reference_t<T>;
		if constexpr (sizeof(m_storage) < sizeof(ValueBox) + sizeof(T_))
		{
			assignBigValue(std::forward(value))
		}
		else
		{
			assignSmallValue(std::forward(value));
		}
	}

	template<typename T>
	bool castToRawPtr(T*& ptr)
	{}

	template<typename T, typename D>
	bool castToUniquePtr(UniquePtr<T, D>& uniquePtr)
	{
		if (vt_unique_ptr == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			T* rawPtr;
			if (castToRawPtr(type, (void**)&rawPtr) && rawPtr == m_ptr)
			{
				uniquePtr.assignRawPtr(rawPtr);
				m_category = vt_null;//owner ship transferred 
				return true;
			}
		}
		return false;
	}

	template<typename T, typename D>
	bool castToUniqueArray(UniqueArray<T, D>& uniquePtr)
	{
		if (vt_unique_array == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			if (type == m_type)
			{
				uniquePtr.assignRawPtr((T*)m_ptr, m_size);
				m_category = vt_null;//owner ship transferred 
				return true;
			}
		}
		return false;
	}

	template<typename T, typename D>
	bool castToSharedPtr(SharedPtr<T, D>& sharedPtr)
	{
		if (vt_shared_ptr == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			T* rawPtr;
			if (castToRawPtr(type, (void**)&rawPtr) && rawPtr == m_ptr)
			{
				sharedPtr.assignRawPtr(rawPtr);
				return true;
			}
		}
		return false;
	}

	template<typename T, typename D>
	bool castToSharedArray(SharedArray<T, D>& sharedArray)
	{
		if (vt_shared_array == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			if (type == m_type)
			{
				sharedArray.assignRawPtr((T*)m_ptr, m_size);
				return true;
			}
		}
		return false;
	}


	template<typename T>
	bool castToBorrowedPtr(BorrowedPtr<T>& borrowedPtr)
	{
		if (vt_borrowed_ptr == m_category || vt_unique_ptr == m_category || vt_shared_ptr == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			T* rawPtr;
			if (castToRawPtr(type, (void**)&rawPtr) && rawPtr == m_ptr)
			{
				borrowedPtr.assignRawPtr(rawPtr);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool castToBorrowedArray(BorrowedArray<T>& borrowedArray)
	{
		if (vt_borrowed_array == m_category || vt_unique_array == m_category || vt_shared_array == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			if (type == m_type)
			{
				borrowedArray.assignRawPtr((T*)m_ptr, m_size);
				return true;
			}
		}
		return false;
	}

	//bool castToValue(Type* dstType, void* dst) const;
	//bool castToReference(Type* dstType, void* dst) const;

	//bool castToVoidPtr(void** dst) const;
	//bool castToPrimitivePtr(Type* dstType, void** dst) const;
	//bool castToEnumPtr(Type* dstType, void** dst) const;
	//bool castToReferencePtr(Type* dstType, void** dst) const;

	//bool castToVoidPtrAllowNull(void** dst) const;
	//bool castToPrimitivePtrAllowNull(Type* dstType, void** dst) const;
	//bool castToEnumPtrAllowNull(Type* dstType, void** dst) const;
	//bool castToValuePtrAllowNull(Type* dstType, void** dst) const;
	//bool castToReferencePtrAllowNull(Type* dstType, void** dst) const;

	//bool castToObjectPtr(Type* dstType, void** dst) const;
	//bool castToObject(Type* dstType, void* dst) const;

	//void reinterpretCastToPtr(Variant& var, Type* dstType) const;

	//void setTemporary();
	//bool isTemporary() const;

	//void setSubClassProxy();
	//bool isSubClassProxy() const;
//public:
//	template<typename T>
//	void assignValue(const T& t)
//	{
//		assignPrimitive(RuntimeTypeOf<T>::RuntimeType::GetInstance(), &t);
//	}

private:
	void assignBorrowedPtr(Type* type, void* ptr);

	void assignBorrowedArray(Type* type, void* ptr, size_t size);

	void assignSharedPtr(Type* type, void* ptr);

	void assignSharedArray(Type* type, void* ptr, size_t size);

	void assignUniquePtr(Type* type, void* ptr);

	void assignUniqueArray(Type* type, void* ptr, size_t size);

	template<typename T>
	void assignBigValue(T&& value)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		m_ptr = ValueBoxImpl<T_>::New(std::forward(value));
		m_category = vt_big_value;
	}

	template<typename T>
	void assignSmallValue(T&& value)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		ValueBoxImpl<T_>::PlacementNew(m_storage, std::forward(value));
		m_category = vt_small_value;
	}
private:
	Category m_category;
	uint32_t m_arrayIndex;
	Type* m_type;
	union 
	{
		struct 
		{
			void* m_ptr;
			size_t m_size;
		};
		byte_t m_storage[storage_size];
	};
};

//------------------------------------------------------------------------------
inline bool Variant::isNull()
{
	return vt_null == m_category;
}

END_PAFCORE
