#pragma once

#include "Utility.h"
#include "SmartPtr.h"

BEGIN_PAF

class Metadata;
class Type;
class PrimitiveType;
class EnumType;
class Introspectable;
class Value;

//enum ErrorLocation
//{
//	unknown,
//	field,
//	property,
//	method_this,
//	method_arg_1,
//	method_arg_2,
//	method_arg_3,
//	method_arg_4,
//	method_arg_5,
//	method_arg_6,
//	method_arg_7,
//	method_arg_8,
//	method_arg_9,
//	method_arg_10,
//	method_arg_11,
//	method_arg_12,
//	method_arg_13,
//	method_arg_14,
//	method_arg_15,
//	method_arg_16,
//	method_arg_17,
//	method_arg_18,
//	method_arg_19,
//	method_arg_20,
//};
//
//struct ErrorInvalidType
//{
//	Type* type;
//};
//
//struct ErrorTypeMismatch
//{
//	Type* dstType;
//	Type* srcType;
//};
//
//struct ErrorTypeCompoundMismatch
//{
//	TypeCompound dstTypeCompound;
//	TypeCompound srcTypeCompound;
//};
//
//struct ErrorMemberNotFound
//{
//	Metadata* scope;
//	const char* memberName;
//};
//
//struct Result
//{
//	ErrorCode errorCode;
//	ErrorLocation errorLocation;
//	union
//	{
//		ErrorInvalidType invalidType;
//		ErrorTypeMismatch typeMismatch;
//		ErrorTypeCompoundMismatch typeCompoundMismatch;
//		ErrorMemberNotFound memberNotFound;
//	};
//public:
//	static Result OK()
//	{
//		Result result;
//		result.errorCode = ErrorCode::s_ok;
//		result.errorLocation = ErrorLocation::unknown;
//		return result;
//	}
//	static Result InvalidType(Type* type)
//	{
//		Result result;
//		result.errorCode = ErrorCode::e_invalid_type;
//		result.errorLocation = ErrorLocation::unknown;
//		result.invalidType.type = type;
//		return result;
//	}
//	static Result TypeMismatch(ErrorLocation errorLocation, Type* dstType, Type* srcType)
//	{
//		Result result;
//		result.errorCode = ErrorCode::e_name_conflict;
//		result.errorLocation = errorLocation;
//		result.typeMismatch.dstType = dstType;
//		result.typeMismatch.srcType = srcType;
//		return result;
//	}
//	static Result TypeCompoundMismatch(ErrorLocation errorLocation, TypeCompound dstTypeCompound, TypeCompound srcTypeCompound)
//	{
//		Result result;
//		result.errorCode = ErrorCode::e_name_conflict;
//		result.errorLocation = errorLocation;
//		result.typeCompoundMismatch.dstTypeCompound = dstTypeCompound;
//		result.typeCompoundMismatch.srcTypeCompound = srcTypeCompound;
//		return result;
//	}
//	static Result MemberNotFound(Metadata* scope, const char* memberName)
//	{
//		Result result;
//		result.errorCode = ErrorCode::e_member_not_found;
//		result.memberNotFound.scope = scope;
//		result.memberNotFound.memberName = memberName;
//		return result;
//	}
//};

struct ValueBox
{
public:
	virtual void destruct()
	{};
	virtual void deallocate() 
	{};
public:
	static void Destruct(void* ptr)
	{
		ValueBox* box = (ValueBox*)(ptr)-1;
		box->destruct();
	}
	static void Delete(void* ptr)
	{
		ValueBox* box = (ValueBox*)(ptr)-1;
		box->destruct();
		box->deallocate();
	}
};

template<typename T>
class ValueBoxImpl : public ValueBox
{
public:
	void destruct() override
	{
		T* ptr = reinterpret_cast<T*>(this + 1);
		ptr->~T();
	}
	void deallocate() override
	{
		free(this);
	}
public:
	template<typename... Types>
	static T* NewBig(Types&&... args)
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
	static T* NewSmall(void* storage, Types&&... args)
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
		vt_reference,
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
private:
	Variant(Variant const& var) = delete;
	Variant& operator = (Variant const& var) = delete;
public:
	Variant();
	Variant(Variant&& var);
	~Variant();
	void assign(Variant&& var);
public:
	Category getCategory() const;
	bool isNull() const;
	bool isValue() const;
	bool isReference() const;
	bool isPointer() const;
	bool isArray() const;
	size_t getArraySize() const;
	Type* getType() const;
	void* getRawPointer() const;
	void clear();
	void assignEnumByInt(EnumType* type, int value);
	bool castToPrimitive(PrimitiveType* dstType, void* dst) const;
	bool castToEnum(EnumType* dstType, void* dst) const;
	bool castToString(string_t& str);
	bool castToBuffer(buffer_t& buf);
	bool castToRawPointer(Type* dstType, void** dst) const;

	ErrorCode newValue(Type* type, ::paf::Variant** args, uint32_t numArgs);
	ErrorCode newUniquePtr(Type* type, ::paf::Variant** args, uint32_t numArgs);
	ErrorCode newUniqueArray(Type* type, size_t count);
	ErrorCode newUniqueArray(Type* type, ::paf::Variant** args, uint32_t numArgs);
	ErrorCode newSharedPtr(Type* type, ::paf::Variant** args, uint32_t numArgs);
	ErrorCode newSharedArray(Type* type, size_t count);
	ErrorCode newSharedArray(Type* type, ::paf::Variant** args, uint32_t numArgs);
	ErrorCode subscript(Variant& var, uint32_t index);
public:
	template <typename T, typename... Types>
	void newValue(Types&&... args)
	{
		using T_ = std::remove_reference_t<T>;
		newValue_<T_>(std::bool_constant<sizeof(ValueBox) + sizeof(T_) <= sizeof(m_storage)>(), std::forward<Types>(args)...);
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
		Type* type = typename RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		assignRawPtr(type, rawPtr.get());
	}

	template<typename T>
	void assignRawArray(RawArray<T> const& rawArray)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		assignRawArray(type, rawArray.get(), rawArray.size());
	}

	template<typename T>
	void assignBorrowedPtr(BorrowedPtr<T> const& borrowedPtr)
	{
		Type* type = RuntimeTypeOf<T>::RuntimeType::GetSingleton();
		borrowedPtr.incBorrowCount();
		assignBorrowedPtr(type, borrowedPtr.get());
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
		borrowedArray.incBorrowCount();
		assignBorrowedArray(type, borrowedArray.get(), borrowedArray.size());
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
		sharedPtr.incOwnerCount();
		assignSharedPtr(type, sharedPtr.get());
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
		sharedArray.incOwnerCount(ptr);
		assignSharedArray(type, sharedArray.get(), sharedArray.size());
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
	void newValue_(std::false_type smallObject, Types&&... args)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton(); RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		m_ptr = ValueBoxImpl<T_>::NewBig(std::forward<Types>(args)...);
		m_category = vt_big_value;
	}

	template <typename T, typename... Types>
	void newValue_(std::true_type smallObject, Types&&... args)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		ValueBoxImpl<T_>::NewSmall(m_storage, std::forward<Types>(args)...);
		m_category = vt_small_value;
	}

	template<typename T>
	void assignValue_(T const& value, std::false_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		m_ptr = ValueBoxImpl<T_>::NewBig(value);
		m_category = vt_big_value;
	}

	template<typename T>
	void assignValue_(T const& value, std::true_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		ValueBoxImpl<T_>::NewSmall(m_storage, value);
		m_category = vt_small_value;
	}

	template<typename T>
	void assignValue_(T&& value, std::false_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		m_ptr = ValueBoxImpl<T_>::NewBig(std::move(value));
		m_category = vt_big_value;
	}

	template<typename T>
	void assignValue_(T&& value, std::true_type smallObject)
	{
		using T_ = std::remove_reference_t<T>;
		clear();
		m_type = RuntimeTypeOf<T_>::RuntimeType::GetSingleton();
		ValueBoxImpl<T_>::NewSmall(m_storage, std::move(value));
		m_category = vt_small_value;
	}

public:
	void assignReference(Type* type, void* ptr);

	void assignRawPtr(Type* type, void* ptr);

	void assignRawArray(Type* type, void* ptr, size_t size);

private:
	void assignBorrowedPtr(Type* type, void* ptr);

	void assignBorrowedArray(Type* type, void* ptr, size_t size);

	void assignSharedPtr(Type* type, void* ptr);

	void assignSharedArray(Type* type, void* ptr, size_t size);

	void assignUniquePtr(Type* type, void* ptr);

	void assignUniqueArray(Type* type, void* ptr, size_t size);

private:
	Category m_category;
	Type* m_type;
	union 
	{
		struct
		{
			void* m_ptr;
			size_t m_arraySize;
			size_t m_arrayIndex;
		};
		byte_t m_storage[storage_size];
	};
};

//------------------------------------------------------------------------------
inline bool Variant::isNull() const
{
	return vt_null == m_category;
}

inline bool Variant::isValue() const
{
	return (vt_small_value == m_category || vt_big_value == m_category);
}

inline bool Variant::isReference() const
{
	return (vt_reference == m_category);
}

inline bool Variant::isPointer() const
{
	return (vt_raw_ptr == m_category || vt_borrowed_ptr == m_category || vt_unique_ptr == m_category || vt_shared_ptr == m_category);
}

inline bool Variant::isArray() const
{
	return (vt_raw_array == m_category || vt_borrowed_array == m_category || vt_unique_array == m_category || vt_shared_array == m_category);
}

inline size_t Variant::getArraySize() const
{
	PAF_ASSERT(isArray());
	return m_arraySize;
}

inline Type* Variant::getType() const 
{
	return m_type;
}

inline Variant::Category Variant::getCategory() const
{
	return m_category;
}

END_PAF
