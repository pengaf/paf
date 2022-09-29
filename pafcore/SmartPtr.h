#pragma once

#include "Utility.h"
#include "Object.h"
#include "DebugHelper.h"
#include <atomic>
#include <type_traits>

#ifdef _DEBUG
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#endif

BEGIN_PAFCORE

template<typename T>
static constexpr bool is_refcounted = std::is_base_of_v<pafcore::RefCounted, T>;

template<typename T1, typename T2>
static constexpr bool convertable_unique_ptr_v = (std::is_base_of_v<T1, T2> || std::is_base_of_v<T2, T1>) && std::has_virtual_destructor_v<T1> && std::has_virtual_destructor_v<T2>;

template<typename T1, typename T2>
static constexpr bool convertable_shared_ptr_v = (std::is_base_of_v<T1, T2> || std::is_base_of_v<T2, T1>) && std::has_virtual_destructor_v<T1> && std::has_virtual_destructor_v<T2> && has_ref_count_interface<T1> && has_ref_count_interface<T2>;

class Box
{
	~Box() = delete;
public:
	virtual void destruct(void* p) = 0;
public:
	void incBorrowCount()
	{
		PAF_ASSERT(!dangling());
		++m_borrow;
	}
	void decBorrowCount()
	{
		PAF_ASSERT(0 < m_borrow);
		if (0 == --m_borrow)
		{
			PAF_ASSERT(0 == m_owner);
			free((void*)this);
		}
	}
	void incOwnerCount()
	{
		PAF_ASSERT(!dangling());
		++m_owner;
	}
	void decOwnerCount()
	{
		if (0 == --m_owner)
		{
			destruct((void*)(this + 1));
			decBorrowCount();
		}
	}
	bool dangling() const
	{
		return 0 == m_owner;
	}
protected:
	mutable long m_owner{ 1 };
	mutable long m_borrow{ 1 };
public:
	static Box* FromRawPtr(const void* ptr)
	{
		return (Box*)(ptr)-1;
	}

	static bool IsDangling(const void* ptr)
	{
		return ptr ? static_cast<const Box*>(ptr)->dangling() : false;
	}

	static void IncBorrowCount(const void* ptr)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->incBorrowCount();
		}
	}

	static void DecBorrowCount(const void* ptr)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->decBorrowCount();
		}
	}

	static void IncOwnerCount(const void* ptr)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->incOwnerCount();
		}
	}

	static void DecOwnerCount(const void* ptr)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->decOwnerCount();
		}
	}
};

class ArrayBox
{
	~ArrayBox() = delete;
public:
	virtual void destructArray(void* p, size_t arraySize) = 0;
public:
	void incBorrowCount()
	{
		PAF_ASSERT(!dangling());
		++m_borrow;
	}
	void decBorrowCount()
	{
		PAF_ASSERT(0 < m_borrow);
		if (0 == --m_borrow)
		{
			PAF_ASSERT(0 == m_owner);
			free((void*)this);
		}
	}
	void incOwnerCount()
	{
		PAF_ASSERT(!dangling());
		++m_owner;
	}
	void decOwnerCount(size_t arraySize)
	{
		if (0 == --m_owner)
		{
			destructArray((void*)(this + 1), arraySize);
			decBorrowCount();
		}
	}
	bool dangling() const
	{
		return 0 == m_owner;
	}
protected:
	mutable long m_owner{ 1 };
	mutable long m_borrow{ 1 };
public:
	static ArrayBox* FromRawPtr(const void* ptr)
	{
		return (ArrayBox*)(ptr)-1;
	}

	static bool IsDangling(const void* ptr)
	{
		return ptr ? static_cast<const ArrayBox*>(ptr)->dangling() : false;
	}

	static void IncBorrowCount(const void* ptr)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->incBorrowCount();
		}
	}

	static void DecBorrowCount(const void* ptr)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->decBorrowCount();
		}
	}

	static void IncOwnerCount(const void* ptr)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->incOwnerCount();
		}
	}

	static void DecOwnerCount(const void* ptr, size_t arraySize)
	{
		if (ptr)
		{
			FromRawPtr(ptr)->decOwnerCount(arraySize);
		}
	}
};


template<typename T>
struct Destructor
{
public:
	static void Destruct(T* ptr)
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			ptr->~T();
		}
	}
	static void DestructArray(T* ptr, size_t arraySize)
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			for (; arraySize > 0; --arraySize)
			{
				ptr->~T();
				++ptr;
			}
		}
	}
};

template<typename T, typename D = Destructor<T>>
class BoxImpl : public Box
{
public:
	void destruct(void* p) override
	{
		D::Destruct(reinterpret_cast<T*>(p));
	}
public:
	template<typename... Types>
	static T* New(Types&&... args)
	{
		size_t size = sizeof(BoxImpl) + sizeof(T);
		void* p = malloc(size);
		BoxImpl* box = (BoxImpl*)p;
		new(box)BoxImpl();
		T* ptr = (T*)(box + 1);
		new(ptr)T(std::forward<Types>(args)...);
		return ptr;
	}
};

template<typename T, typename D = Destructor<T>>
class ArrayBoxImpl : public ArrayBox
{
public:
	void destructArray(void* p, size_t arraySize) override
	{
		D::DestructArray(reinterpret_cast<T*>(p), arraySize);
	}
public:
	static T* NewArray(size_t arraySize)
	{
		size_t size = sizeof(ArrayBoxImpl) + sizeof(T) * arraySize;
		void* p = malloc(size);
		ArrayBoxImpl* box = (ArrayBoxImpl*)p;
		new(box)ArrayBoxImpl();
		T* ptr = (T*)(box + 1);
		new(ptr)T[arraySize];
		return ptr;
	}
};

template<typename T, typename D = Destructor<T>>
class UniquePtr;

template<typename T, typename D = Destructor<T>>
class UniqueArray;

template<typename T, typename D = Destructor<T>>
class SharedPtr;

template<typename T, typename D = Destructor<T>>
class SharedArray;

template<typename T>
class BorrowedPtr;

template<typename T>
class BorrowedArray;

class Variant;

template<typename T, typename D>
class UniquePtr
{
	template <typename T2>
	friend class BorrowedPtr;

	template <typename T2, typename D2>
	friend class UniquePtr;

	friend class GenericUniquePtr;

public:
	using element_type = T;
	using pointer = T*;
	using reference = T&;
private:
	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;
private:
	explicit UniquePtr(pointer ptr) noexcept :
		m_ptr(ptr)
	{}
public:
	constexpr UniquePtr() noexcept :
		m_ptr(nullptr)
	{}

	constexpr UniquePtr(nullptr_t) noexcept :
		m_ptr(nullptr)
	{}

	UniquePtr(UniquePtr&& other) noexcept :
		m_ptr(other.get())
	{
		other.m_ptr = nullptr;
	}

	template<typename T2, typename D2, std::enable_if_t<convertable_unique_ptr_v<T, T2>, int> = 0>
	UniquePtr(UniquePtr<T2, D2>&& other) noexcept :
		m_ptr(static_cast<T*>(other.get()))
	{
		PAF_ASSERT(paf_base_offset_of(T, T2) == 0);
		other.m_ptr = nullptr;
	}

	~UniquePtr() noexcept
	{
		Box::DecOwnerCount(m_ptr);
	}

	UniquePtr& operator=(nullptr_t other) noexcept
	{
		Box::DecOwnerCount(m_ptr);
		m_ptr = nullptr;
		return *this;
	}

	UniquePtr& operator=(UniquePtr&& other) noexcept
	{
		if (this != std::addressof(other))
		{
			Box::DecOwnerCount(m_ptr);
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}
		return *this;
	}

	template<typename T2, typename D2, std::enable_if_t<convertable_unique_ptr_v<T, T2>, int> = 0>
	UniquePtr& operator=(UniquePtr<T2, D2>&& other) noexcept
	{
		PAF_ASSERT(paf_base_offset_of(T, T2) == 0);
		Box::DecOwnerCount(m_ptr);
		m_ptr = static_cast<T*>(other.m_ptr);
		other.m_ptr = nullptr;
		return *this;
	}

	reference operator*() const noexcept
	{
		PAF_ASSERT(m_ptr);
		return *m_ptr;
	}

	pointer operator->() const noexcept 
	{
		PAF_ASSERT(m_ptr);
		return m_ptr;
	}

	explicit operator bool() const noexcept
	{
		return static_cast<bool>(m_ptr);
	}

	bool operator==(const T* ptr) const
	{
		return m_ptr == ptr;
	}

	bool operator!=(const T* ptr) const
	{
		return m_ptr != ptr;
	}

	template<typename T2>
	bool operator==(const BorrowedPtr<T2>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T2>
	bool operator!=(const BorrowedPtr<T2>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator==(const UniquePtr<T2, D2>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator!=(const UniquePtr<T2, D2>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	pointer get() const noexcept
	{
		return m_ptr;
	}

	void swap(UniquePtr& other) noexcept
	{
		std::swap(m_ptr, other.m_ptr);
	}
private:
	void assignRawPtr(pointer ptr)
	{
		Box::DecOwnerCount(m_ptr);
		m_ptr = ptr;
	}
private:
	T* m_ptr;
public:
	template <typename... Types>
	static UniquePtr Make(Types&&... args)
	{
		T* p = BoxImpl<T, D>::New(std::forward<Types>(args)...);
		return UniquePtr(p);
	}
};

template<typename T, typename D>
class UniqueArray
{
	friend class BorrowedArray<T>;
public:
	using element_type = T;
	using pointer = T*;
	using reference = T&;
	using size_type = size_t;
private:
	UniqueArray(const UniqueArray&) = delete;
	UniqueArray& operator=(const UniqueArray&) = delete;
	UniqueArray(pointer ptr, size_type size) noexcept :
		m_ptr(ptr),
		m_size(size)
	{}
public:
	constexpr UniqueArray() noexcept :
		m_ptr(nullptr),
		m_size(0)
	{}

	constexpr UniqueArray(nullptr_t) noexcept :
		m_ptr(nullptr),
		m_size(0)
	{}

	UniqueArray(UniqueArray&& other) noexcept :
		m_ptr(other.m_ptr),
		m_size(other.m_size)
	{
		other.m_ptr = nullptr;
		other.m_size = 0;
	}

	~UniqueArray() noexcept
	{
		decOwnerCount();
	}

	UniqueArray& operator=(nullptr_t other) noexcept
	{
		decOwnerCount();
		m_ptr = nullptr;
		m_size = 0;
		return *this;
	}

	UniqueArray& operator=(UniqueArray&& other) noexcept
	{
		if (this != std::addressof(other))
		{
			m_ptr = other.m_ptr;
			m_size = other.m_size;
			other.m_ptr = nullptr;
			other.m_size = 0;
		}
		return *this;
	}

	reference operator[](size_t idx) const noexcept
	{
		PAF_ASSERT(m_ptr && idx < m_size);
		return m_ptr[idx];
	}

	explicit operator bool() const noexcept
	{
		return static_cast<bool>(m_ptr);
	}

	bool operator==(const T* ptr) const
	{
		return m_ptr == ptr;
	}

	bool operator!=(const T* ptr) const
	{
		return m_ptr != ptr;
	}

	bool operator==(const BorrowedArray<T>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const BorrowedArray<T>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	bool operator==(const UniqueArray& other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const UniqueArray& other) const
	{
		return m_ptr != other.m_ptr;
	}

	pointer get() const noexcept
	{
		return m_ptr;
	}

	pointer get(size_t idx) const noexcept
	{
		PAF_DEBUG_ONLY(0 == idx || idx < m_size);
		return idx < m_size ? m_ptr + idx : nullptr;
	}

	size_type size() const noexcept
	{
		return m_size;
	}

	void swap(UniqueArray& other) noexcept
	{
		std::swap(m_ptr, other.m_ptr);
		std::swap(m_size, other.m_size);
	}
private:
	void assignRawPtr(pointer ptr, size_t size)
	{
		ArrayBox::DecOwnerCount(m_ptr, m_size);
		m_ptr = ptr;
		m_size = size;
	}
private:
	void incOwnerCount()
	{
		ArrayBox::IncOwnerCount(m_ptr);
	}
	void decOwnerCount()
	{
		ArrayBox::DecOwnerCount(m_ptr, m_size);
	}
private:
	T* m_ptr;
	size_t m_size;
public:
	static UniqueArray Make(const size_t size)
	{
		T* p = ArrayBoxImpl<T, D>::NewArray(size);
		return UniqueArray(p, size);
	}
};


template<typename T, typename D>
class SharedPtr
{
	template <typename T2>
	friend class BorrowedPtr;

	template<typename T2, typename D2>
	friend class SharedPtr;

	friend class GenericSharedPtr;
public:
	using pointer = T*;
	using reference = T&;
private:
	explicit SharedPtr(pointer ptr) noexcept :
		m_ptr(ptr)
	{}
	//for shared from other raw pointer
	SharedPtr(pointer ptr, std::true_type tag) noexcept :
		m_ptr(ptr)
	{
		incRefCount();
	}
public:
	constexpr SharedPtr() noexcept :
		m_ptr(nullptr)
	{}

	constexpr SharedPtr(nullptr_t) noexcept :
		m_ptr(nullptr)
	{}

	SharedPtr(const SharedPtr& other) noexcept :
		m_ptr(other.m_ptr)
	{
		incOwnerCount();
	}

	template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
	SharedPtr(const SharedPtr<T2, D2>& other) noexcept :
		m_ptr(other.m_ptr)
	{
		PAF_ASSERT(paf_base_offset_of(T, T2) == 0);
		incOwnerCount();
	}

	SharedPtr(SharedPtr&& other) noexcept :
		m_ptr(other.m_ptr)
	{
		other.m_ptr = nullptr;
	}

	template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
	SharedPtr(SharedPtr<T2, D2>&& other) noexcept :
		m_ptr(static_cast<T*>(other.m_ptr))
	{
		PAF_ASSERT(paf_base_offset_of(T, T2) == 0);
		other.m_ptr = nullptr;
	}

	~SharedPtr() noexcept
	{
		decOwnerCount();
	}

	SharedPtr& operator=(nullptr_t other) noexcept
	{
		decOwnerCount();
		m_ptr = nullptr;
		return *this;
	}

	SharedPtr& operator=(const SharedPtr& other) noexcept
	{
		SharedPtr(other).swap(*this);
		return *this;
	}

	template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
	SharedPtr& operator=(const SharedPtr<T2, D2>& other) noexcept
	{
		PAF_ASSERT(paf_base_offset_of(T, T2) == 0);
		SharedPtr(other).swap(*this);
		return *this;
	}

	SharedPtr& operator=(SharedPtr&& other) noexcept
	{
		//SharedPtr(std::move(other)).swap(*this);
		decOwnerCount();
		m_ptr = other.m_ptr;
		other.m_ptr = nullptr;
		return *this;
	}

	template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
	SharedPtr& operator=(SharedPtr<T2, D2>&& other) noexcept
	{
		PAF_ASSERT(paf_base_offset_of(T, T2) == 0);
		//SharedPtr(std::move(other)).swap(*this);
		decOwnerCount();
		m_ptr = static_cast<T*>(other.m_ptr);
		other.m_ptr = nullptr;
		return *this;
	}

	reference operator*() const noexcept
	{
		PAF_ASSERT(m_ptr);
		return *m_ptr;
	}

	pointer operator->() const noexcept
	{
		PAF_ASSERT(m_ptr);
		return m_ptr;
	}

	explicit operator bool() const noexcept
	{
		return static_cast<bool>(m_ptr);
	}

	bool operator==(const T* ptr) const
	{
		return m_ptr == ptr;
	}

	bool operator!=(const T* ptr) const
	{
		return m_ptr != ptr;
	}

	template<typename T2>
	bool operator==(const BorrowedPtr<T2>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T2>
	bool operator!=(const BorrowedPtr<T2>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator==(const SharedPtr<T2, D2>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator!=(const SharedPtr<T2, D2>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	pointer get() const noexcept
	{
		return m_ptr;
	}

	void swap(SharedPtr& other) noexcept
	{
		std::swap(m_ptr, other.m_ptr);
	}
private:
	void assignRawPtr(pointer ptr)
	{
		Box::IncOwnerCount(ptr);
		Box::DecOwnerCount(m_ptr);
		m_ptr = ptr;
	}
private:
	void incOwnerCount()
	{
		Box::IncOwnerCount(m_ptr);
	}
	void decOwnerCount()
	{
		Box::DecOwnerCount(m_ptr);
	}
	bool isDangling()
	{
		return Box::IsDangling(m_ptr);
	}
private:
	T* m_ptr;
public:
	template <typename... Types>
	static SharedPtr Make(Types&&... args)
	{
		T* p = BoxImpl<T, D>::New(std::forward<Types>(args)...);
		return SharedPtr(p);
	}
	static SharedPtr SharedFrom(T* ptr)
	{
		return SharedPtr(p, std::true_type());
	}
};


template<typename T, typename D>
class SharedArray
{
	friend class BorrowedArray<T>;
public:
	using pointer = T*;
	using reference = T&;
	using size_type = size_t;
private:
	SharedArray(pointer ptr, size_type size) noexcept :
		m_ptr(ptr),
		m_size(size)
	{}
public:
	constexpr SharedArray() noexcept :
		m_ptr(nullptr),
		m_size(0)
	{}

	constexpr SharedArray(nullptr_t) noexcept :
		m_ptr(nullptr),
		m_size(0)
	{}

	SharedArray(SharedArray const& other) noexcept :
		m_ptr(other.m_ptr),
		m_size(other.m_size)
	{
		incOwnerCount();
	}

	SharedArray(SharedArray&& other) noexcept :
		m_ptr(other.m_ptr),
		m_size(other.m_size)
	{
		other.m_ptr = nullptr;
		other.m_size = 0;
	}

	~SharedArray() noexcept
	{
		decOwnerCount();
	}

	SharedArray& operator=(SharedArray const& other) noexcept
	{
		SharedArray(other).swap(*this);
		return *this;
	}

	SharedArray& operator=(SharedArray&& other) noexcept
	{
		SharedArray(std::move(other)).swap(*this);
		return *this;
	}

	reference operator[](size_t idx) const noexcept
	{
		PAF_ASSERT(m_ptr && idx < m_size);
		return m_ptr[idx];
	}

	explicit operator bool() const noexcept
	{
		return static_cast<bool>(m_ptr);
	}

	bool operator==(const T* ptr) const
	{
		return m_ptr == ptr;
	}

	bool operator!=(const T* ptr) const
	{
		return m_ptr != ptr;
	}

	bool operator==(const BorrowedArray<T>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const BorrowedArray<T>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	bool operator==(const SharedArray<T>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const SharedArray<T>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	pointer get() const noexcept
	{
		return m_ptr;
	}

	pointer get(size_t idx) const noexcept
	{
		PAF_DEBUG_ONLY(0 == idx || idx < m_size);
		return idx < m_size ? m_ptr + idx : nullptr;
	}

	size_type size() const noexcept
	{
		return m_size;
	}

	void swap(SharedArray& other) noexcept
	{
		PAF_DEBUG_ONLY(SharedPtrBase::swap(other));
		std::swap(m_ptr, other.m_ptr);
		std::swap(m_size, other.m_size);
	}
private:
	void assignRawPtr(pointer ptr, size_t size)
	{
		ArrayBox::IncOwnerCount(ptr);
		ArrayBox::DecOwnerCount(m_ptr, m_size);
		m_ptr = ptr;
		m_size = size;
	}
private:
	void incOwnerCount()
	{
		ArrayBox::IncOwnerCount(m_ptr);
	}
	void decOwnerCount()
	{
		ArrayBox::DecOwnerCount(m_ptr, m_size);
	}
private:
	pointer m_ptr;
	size_t m_size;
public:
	static SharedArray Make(const size_t size)
	{
		T* p = ArrayBoxImpl<T, D>::NewArray(size);
		return SharedArray(p, size);
	}
};


template<typename T>
class BorrowedPtr
{
	template <typename T2>
	friend class BorrowedPtr;

	template <typename T2, typename D2>
	friend class UniquePtr;

	template<typename T2, typename D2>
	friend class SharedPtr;

	friend class Variant;
public:
	using element_type = T;
	using pointer = T*;
	using reference = T&;
public:
	constexpr BorrowedPtr() noexcept :
		m_ptr(nullptr)
	{}

	constexpr BorrowedPtr(nullptr_t) noexcept :
		m_ptr(nullptr)
	{}

	BorrowedPtr(const BorrowedPtr& other) noexcept :
		m_ptr(other.m_ptr)
	{
		incBorrowCount();
	}

	template<typename T2>
	BorrowedPtr(const BorrowedPtr<T2>& other) noexcept :
		m_ptr(other.m_ptr)
	{
		incBorrowCount();
	}

	template<typename T2, typename D2>
	explicit BorrowedPtr(const UniquePtr<T2, D2>& other) noexcept :
		m_ptr(other.m_ptr)
	{
		incBorrowCount();
	}

	template<typename T2, typename D2>
	explicit BorrowedPtr(const SharedPtr<T2, D2>& other) noexcept :
		m_ptr(other.m_ptr)
	{
		incBorrowCount();
	}

	BorrowedPtr& operator=(nullptr_t other) noexcept
	{
		decBorrowCount();
		m_ptr = nullptr;
		return *this;
	}

	BorrowedPtr& operator=(const BorrowedPtr& other) noexcept
	{
		other.incBorrowCount();
		if (this != std::addressof(other))
		{
			reset(othet.get());
		}
		return *this;
	}

	template<typename T2>
	BorrowedPtr& operator=(const BorrowedPtr<T2>& other) noexcept
	{
		reset(othet.get());
		return *this;
	}

	template<typename T2, typename D2>
	BorrowedPtr& operator=(const UniquePtr<T2, D2>& other) noexcept
	{
		reset(othet.get());
		return *this;
	}

	template<typename T2, typename D2>
	BorrowedPtr& operator=(const SharedPtr<T2, D2>& other) noexcept
	{
		reset(othet.get());
		return *this;
	}

	reference operator*() const noexcept
	{
		PAF_DEBUG_ONLY(!isDangling());
		PAF_ASSERT(m_ptr);
		return *m_ptr;
	}

	pointer operator->() const noexcept
	{
		PAF_DEBUG_ONLY(!isDangling());
		PAF_ASSERT(m_ptr);
		return m_ptr;
	}

	explicit operator bool() const noexcept
	{
		return static_cast<bool>(m_ptr);
	}

	bool operator==(const T* ptr) const
	{
		return m_ptr == ptr;
	}

	bool operator!=(const T* ptr) const
	{
		return m_ptr != ptr;
	}

	template<typename T2>
	bool operator==(const BorrowedPtr<T2>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T2>
	bool operator!=(const BorrowedPtr<T2>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator==(const UniquePtr<T2, D2>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator!=(const UniquePtr<T2, D2>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator==(const SharedPtr<T2, D2>& other) const
	{
		return m_ptr == other.m_ptr;
	}

	template<typename T2, typename D2>
	bool operator!=(const SharedPtr<T2, D2>& other) const
	{
		return m_ptr != other.m_ptr;
	}

	pointer get() const noexcept
	{
		PAF_DEBUG_ONLY(!isDangling());
		return m_ptr;
	}
private:
	void assignRawPtr(pointer ptr)
	{
		PAF_DEBUG_ONLY(Box::IncBorrowCount(ptr));
		PAF_DEBUG_ONLY(Box::DecBorrowCount(m_ptr));
		m_ptr = ptr;
	}
private:
	void incBorrowCount()
	{
		PAF_DEBUG_ONLY(Box::IncBorrowCount(m_ptr));
	}
	void decBorrowCount()
	{
		PAF_DEBUG_ONLY(Box::DecBorrowCount(m_ptr));
	}
	void reset(T* ptr)
	{
		decBorrowCount();
		m_ptr = ptr;
		incBorrowCount();
	}
	bool isDangling()
	{
		return Box::IsDangling(m_ptr);
	}
private:
	T* m_ptr;
};


template<typename T>
class BorrowedArray
{
	template <typename T2>
	friend class BorrowedArray;

	template <typename T2, typename D2>
	friend class UniqueArray;

	template<typename T2, typename D2>
	friend class SharedArray;

	friend class Variant;
public:
	using element_type = T;
	using pointer = T*;
	using reference = T&;
	using size_type = size_t;
public:
	constexpr BorrowedArray() noexcept :
		m_ptr(nullptr),
		m_size(0)
	{}

	BorrowedArray(const BorrowedArray& other) noexcept :
		m_ptr(other.get()),
		m_size(other.size())
	{
		incBorrowCount();
	}

	BorrowedArray(BorrowedArray&& other) noexcept :
		m_ptr(other.get()),
		m_size(other.size())
	{
		other.m_ptr = nullptr;
		other.m_size = 0;
	}

	explicit BorrowedArray(const UniqueArray<T>&other) noexcept :
		m_ptr(other.get()),
		m_size(other.size())
	{
		incBorrowCount();
	}

	explicit BorrowedArray(const SharedArray<T>&other) noexcept :
		m_ptr(other.get()),
		m_size(other.size())
	{
		incBorrowCount();
	}

	BorrowedArray& operator=(nullptr_t other) noexcept
	{
		decBorrowCount();
		m_ptr = nullptr;
		m_size = 0;
		return *this;
	}

	BorrowedArray& operator=(const BorrowedArray & other) noexcept
	{
		if (this != std::addressof(other))
		{
			m_ptr = other.get();
			m_size = other.size();
		}
		return *this;
	}

	BorrowedArray& operator=(const UniqueArray<T>&other) noexcept
	{
		m_ptr = other.get();
		m_size = other.size();
		return *this;
	}

	BorrowedArray& operator=(const SharedArray<T>&other) noexcept
	{
		m_ptr = other.get();
		m_size = other.size();
		return *this;
	}

	reference operator[](size_t idx) const noexcept
	{
		PAF_DEBUG_ONLY(!isDangling());
		PAF_ASSERT(m_ptr && idx < m_size);
		return m_ptr[idx];
	}

	explicit operator bool() const noexcept
	{
		return static_cast<bool>(m_ptr);
	}

	bool operator==(const T * ptr) const
	{
		return m_ptr == ptr;
	}

	bool operator!=(const T * ptr) const
	{
		return m_ptr != ptr;
	}

	bool operator==(const BorrowedArray & other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const BorrowedArray & other) const
	{
		return m_ptr != other.m_ptr;
	}

	bool operator==(const UniqueArray<T>&other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const UniqueArray<T>&other) const
	{
		return m_ptr != other.m_ptr;
	}

	bool operator==(const SharedArray<T>&other) const
	{
		return m_ptr == other.m_ptr;
	}

	bool operator!=(const SharedArray<T>&other) const
	{
		return m_ptr != other.m_ptr;
	}

	pointer get() const noexcept
	{
		PAF_DEBUG_ONLY(!isDangling());
		return m_ptr;
	}

	pointer get(size_t idx) const noexcept
	{
		PAF_DEBUG_ONLY(!isDangling());
		PAF_DEBUG_ONLY(0 == idx || idx < m_size);
		return idx < m_size ? m_ptr + idx : nullptr;
	}

	size_type size() const noexcept
	{
		return m_size;
	}
private:
	void assignRawPtr(pointer ptr, size_t size)
	{
		PAF_DEBUG_ONLY(Box::IncBorrowCount(ptr));
		PAF_DEBUG_ONLY(Box::DecBorrowCount(m_ptr));
		m_ptr = ptr;
		m_size = size;
	}
private:
	void incBorrowCount()
	{
		PAF_DEBUG_ONLY(ArrayBox::IncBorrowCount(m_ptr));
	}
	void decBorrowCount()
	{
		PAF_DEBUG_ONLY(ArrayBox::DecBorrowCount(m_ptr));
	}
private:
	T* m_ptr;
	size_t m_size;
};


END_PAFCORE
