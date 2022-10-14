#pragma once

#include "Utility.h"
#include "SmartPtr.h"

BEGIN_PAFCORE

class Type;
class PrimitiveType;
class EnumType;
class Introspectable;
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
	static constexpr size_t storage_size = 64 + sizeof(ValueBox);
	enum Category
	{
		vt_null,
		vt_small_value,
		vt_big_value,
		vt_raw_ptr,
		vt_raw_array,
		vt_borrowed_ptr,
		vt_borrowed_array,
		vt_unique_ptr,
		vt_unique_array,
		vt_shared_ptr,
		vt_shared_array,
		vt_borrowed_array_element,
	};
public:
	Variant();
	~Variant();
private:
	Variant(Variant const& var) = delete;
	Variant& operator = (Variant const& var) = delete;
public:
	bool isNull() const;
	Type* getType() const;
	void* getRawPointer() const;
	void clear();
	bool subscript(Variant& var, uint32_t index);

	void assignEnumByInt(EnumType* type, int value);
	bool castToPrimitive(PrimitiveType* dstType, void* dst) const;
	bool castToEnum(EnumType* dstType, void* dst) const;
	bool castToRawPointer(Type* dstType, void** dst) const;

public:
	template <typename T, typename... Types>
	void makeValue(Types&&... args)
	{
		using T_ = std::remove_reference_t<T>;
		makeValue_<T_>(std::bool_constant<sizeof(ValueBox) + sizeof(T_) <= sizeof(m_storage)>(), std::forward<Types>(args)...);
	}

	template<typename T>
	void assignValue(T const& value)
	{
		using T_ = std::remove_reference_t<T>;
		assignValue_(value, std::bool_constant<sizeof(ValueBox) + sizeof(T_) <= sizeof(m_storage)>());
	}

	template<typename T>
	void assignValue(T&& value)
	{
		using T_ = std::remove_reference_t<T>;
		assignValue_(std::move(value), std::bool_constant<sizeof(ValueBox) + sizeof(T_) <= sizeof(m_storage)>());
	}

	template<typename T>
	void assignRawPtr(RawPtr<T> const& rawPtr)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = rawPtr.get();
		assignRawPtr(type, ptr);
	}

	template<typename T>
	void assignRawArray(RawArray<T> const& rawArray)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		T* ptr = rawArray.get();
		size_t size = rawArray.size();
		assignRawArray(type, ptr, size);
	}

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
		uniqueArray.m_ptr = nullptr;
		uniqueArray.m_size = 0;
		assignUniqueArray(type, ptr, size);
	}

	template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	bool castToPrimitive(T& dst) const
	{
		return castToPrimitive(static_cast<PrimitiveType*>(RuntimeTypeOf<T>::RuntimeType::GetSingleton()), (void*)&dst);
	}

	template<typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
	bool castToEnum(T& dst) const
	{
		return castToEnum(static_cast<EnumType*>(RuntimeTypeOf<T>::RuntimeType::GetSingleton()), (void*)&dst);
	}

	template<typename T>
	bool castToRawPointer(T*& rawPointer) const
	{
		return castToRawPointer(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), (void**)&rawPointer);
	}

	template<typename T>
	bool castToRawPtr(RawPtr<T>& rawPtr) const
	{
		if (vt_raw_ptr == m_category || vt_borrowed_ptr == m_category || vt_unique_ptr == m_category || vt_shared_ptr == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			T* rawPointer;
			if (castToRawPointer(type, (void**)&rawPointer))
			{
				rawPtr.assignRawPointer((T*)m_ptr);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool castToRawArray(RawArray<T>& rawArray) const
	{
		if (vt_raw_array == m_category || vt_borrowed_array == m_category || vt_unique_array == m_category || vt_shared_array == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			T* rawPointer;
			if (castToRawPointer(type, (void**)&rawPointer))
			{
				rawArray.assignRawPointer((T*)m_ptr, m_arraySize);
				return true;
			}
		}
		return false;
	}

	template<typename T, typename D>
	bool castToUniquePtr(UniquePtr<T, D>& uniquePtr)
	{
		if (vt_unique_ptr == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			T* rawPointer;
			if (castToRawPointer(type, (void**)&rawPointer))
			{
				uniquePtr.assignRawPointer(rawPointer);
				m_category = vt_null;//owner ship transferred 
				return true;
			}
		}
		return false;
	}

	template<typename T, typename D>
	bool castToUniqueArray(UniqueArray<T, D>& uniqueArray)
	{
		if (vt_unique_array == m_category)
		{
			Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
			if (type == m_type)
			{
				uniqueArray.assignRawPointer((T*)m_ptr, m_arraySize);
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
			T* rawPointer;
			if (castToRawPointer(type, (void**)&rawPointer))
			{
				sharedPtr.assignRawPointer(rawPointer);
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
				sharedArray.assignRawPointer((T*)m_ptr, m_arraySize);
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
			T* rawPointer;
			if (castToRawPointer(type, (void**)&rawPointer))
			{
				borrowedPtr.assignRawPointer(rawPointer);
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
				borrowedArray.assignRawPointer((T*)m_ptr, m_arraySize);
				return true;
			}
		}
		return false;
	}

private:
	template <typename T, typename... Types>
	void makeValue_(std::false_type smallObject, Types&&... args)
	{
		clear();
		m_type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		m_ptr = ValueBoxImpl<T>::New(std::forward<Types>(args)...);
		m_category = vt_big_value;
	}

	template <typename T, typename... Types>
	void makeValue_(std::true_type smallObject, Types&&... args)
	{
		clear();
		m_type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		ValueBoxImpl<T_>::PlacementNew(m_storage, std::forward<Types>(args)...);
		m_category = vt_small_value;
	}

	template<typename T>
	void assignValue_(T const& value, std::false_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		m_ptr = ValueBoxImpl<T_>::New(value);
		m_category = vt_big_value;
	}

	template<typename T>
	void assignValue_(T const& value, std::true_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		ValueBoxImpl<T_>::PlacementNew(m_storage, value);
		m_category = vt_small_value;
	}

	template<typename T>
	void assignValue_(T&& value, std::false_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		m_ptr = ValueBoxImpl<T_>::New(std::move(value));
		m_category = vt_big_value;
	}

	template<typename T>
	void assignValue_(T&& value, std::true_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		ValueBoxImpl<T_>::PlacementNew(m_storage, std::move(value));
		m_category = vt_small_value;
	}
private:
	void assignRawPtr(Type* type, void* ptr);

	void assignRawArray(Type* type, void* ptr, size_t size);

	void assignBorrowedPtr(Type* type, void* ptr);

	void assignBorrowedArray(Type* type, void* ptr, size_t size);

	void assignSharedPtr(Type* type, void* ptr);

	void assignSharedArray(Type* type, void* ptr, size_t size);

	void assignUniquePtr(Type* type, void* ptr);

	void assignUniqueArray(Type* type, void* ptr, size_t size);

private:
	Category m_category;
	uint32_t m_arrayIndex;
	Type* m_type;
	union 
	{
		struct 
		{
			void* m_ptr;
			size_t m_arraySize;
		};
		byte_t m_storage[storage_size];
	};
};

//------------------------------------------------------------------------------
inline bool Variant::isNull() const
{
	return vt_null == m_category;
}

inline Type* Variant::getType() const 
{
	return m_type;
}

END_PAFCORE
