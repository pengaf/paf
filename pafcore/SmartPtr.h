#pragma once

#include "Utility.h"
#include <atomic>
#include <type_traits>

namespace paf
{
	template<typename T>
	static constexpr bool is_interface = std::is_base_of_v<::paf::Interface, T>;

	template<typename T1, typename T2>
	static constexpr bool convertable_unique_ptr_v = (std::is_base_of_v<T1, T2> || std::is_base_of_v<T2, T1>) && std::has_virtual_destructor_v<T1> && std::has_virtual_destructor_v<T2>;

	template<typename T1, typename T2>
	static constexpr bool convertable_shared_ptr_v = (std::is_base_of_v<T1, T2> || std::is_base_of_v<T2, T1>) && std::has_virtual_destructor_v<T1> && std::has_virtual_destructor_v<T2>;


	class Box
	{
	public:
		virtual void destruct() = 0;
		virtual void deallocate() = 0;
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
				deallocate();
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
				destruct();
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
			PAF_ASSERT(ptr);
			return (Box*)(ptr)-1;
		}

		static bool IsDangling(const void* ptr)
		{
			return ptr ? static_cast<const Box*>(ptr)->dangling() : false;
		}

		static void IncBorrowCount(const void* ptr)
		{
			FromRawPtr(ptr)->incBorrowCount();
		}

		static void DecBorrowCount(const void* ptr)
		{
			FromRawPtr(ptr)->decBorrowCount();
		}

		static void IncOwnerCount(const void* ptr)
		{
			FromRawPtr(ptr)->incOwnerCount();
		}

		static void DecOwnerCount(const void* ptr)
		{
			FromRawPtr(ptr)->decOwnerCount();
		}
	};

	class ArrayBox
	{
	public:
		virtual void destructArray(size_t arraySize) = 0;
		virtual void deallocate() = 0;
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
				deallocate();
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
				destructArray(arraySize);
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
			PAF_ASSERT(ptr);
			return (ArrayBox*)(ptr)-1;
		}

		static bool IsDangling(const void* ptr)
		{
			return ptr ? static_cast<const ArrayBox*>(ptr)->dangling() : false;
		}

		static void IncBorrowCount(const void* ptr)
		{
			FromRawPtr(ptr)->incBorrowCount();
		}

		static void DecBorrowCount(const void* ptr)
		{
			FromRawPtr(ptr)->decBorrowCount();
		}

		static void IncOwnerCount(const void* ptr)
		{
			FromRawPtr(ptr)->incOwnerCount();
		}

		static void DecOwnerCount(const void* ptr, size_t arraySize)
		{
			FromRawPtr(ptr)->decOwnerCount(arraySize);
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
		BoxImpl() = default;
	public:
		void destruct() override
		{
			D::Destruct(reinterpret_cast<T*>(this + 1));
		}
		void deallocate() override
		{
			free(this);
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
		void destructArray(size_t arraySize) override
		{
			D::DestructArray(reinterpret_cast<T*>(this + 1), arraySize);
		}
		void deallocate() override
		{
			free(this);
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

	class GenericPtr
	{
	public:
		void* m_ptr;
	};


	class GenericArray
	{
	public:
		void* m_ptr;
		size_t m_size;
	};


	template<typename T, typename D>
	class SharedPtr
	{
		template <typename T2>
		friend class BorrowedPtr;

		template<typename T2, typename D2>
		friend class UniquePtr;

		template<typename T2, typename D2>
		friend class SharedPtr;

		friend class Variant;

		static_assert(sizeof(SharedPtr) == sizeof(GenericPtr));
	public:
		using pointer = T * ;
		using reference = T & ;
	private:
		explicit SharedPtr(pointer ptr) noexcept :
			m_ptr(ptr)
		{}
		//for shared from other raw pointer
		SharedPtr(pointer ptr, std::true_type tag) noexcept :
			m_ptr(ptr)
		{
			incOwnerCount();
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
			m_ptr(static_cast<T*>(other.m_ptr))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
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
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
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
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			SharedPtr(other).swap(*this);
			return *this;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept
		{
			decOwnerCount();
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			return *this;
		}

		template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
		SharedPtr& operator=(SharedPtr<T2, D2>&& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
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
		//for variant
		void assignRawPointer(pointer ptr)
		{
			SharedPtr(ptr, std::true_type()).swap(*this);
		}
		Box* getBox() const
		{
			return m_ptr ? Box::FromRawPtr(m_ptr) : nullptr;
		}
	private:
		void incOwnerCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::IncOwnerCount(m_ptr->getAddress());
				}
				else
				{
					Box::IncOwnerCount(m_ptr);
				}
			}
		}
		void decOwnerCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::DecOwnerCount(m_ptr->getAddress());
				}
				else
				{
					Box::DecOwnerCount(m_ptr);
				}
			}
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
		template <typename T2>
		friend class BorrowedArray;

		template<typename T2, typename D2>
		friend class SharedArray;

		template<typename T2, typename D2>
		friend class UniqueArray;

		friend class Variant;

		static_assert(sizeof(SharedArray) == sizeof(GenericArray));
	public:
		using pointer = T * ;
		using reference = T & ;
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
			PAF_ASSERT(0 == idx || idx < m_size);
			return idx < m_size ? m_ptr + idx : nullptr;
		}

		size_type size() const noexcept
		{
			return m_size;
		}

		void swap(SharedArray& other) noexcept
		{
			std::swap(m_ptr, other.m_ptr);
			std::swap(m_size, other.m_size);
		}
	private:
		//for variant
		void assignRawPointer(pointer ptr, size_t size)
		{
			ArrayBox::IncOwnerCount(ptr);
			ArrayBox::DecOwnerCount(m_ptr, m_size);
			m_ptr = ptr;
			m_size = size;
		}
	private:
		void incOwnerCount()
		{
			if (m_ptr)
			{
				ArrayBox::IncOwnerCount(m_ptr);
			}
		}
		void decOwnerCount()
		{
			if (m_ptr)
			{
				ArrayBox::DecOwnerCount(m_ptr, m_size);
			}
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


	template<typename T, typename D>
	class UniquePtr
	{
		template <typename T2>
		friend class BorrowedPtr;

		template <typename T2, typename D2>
		friend class UniquePtr;

		template <typename T2, typename D2>
		friend class SharedPtr;

		friend class Variant;

	public:
		using element_type = T;
		using pointer = T * ;
		using reference = T & ;
	private:
		UniquePtr(const UniquePtr&) = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;
	public:
		constexpr UniquePtr() noexcept :
			m_ptr(nullptr)
		{}

		constexpr UniquePtr(nullptr_t) noexcept :
			m_ptr(nullptr)
		{}

		explicit UniquePtr(pointer ptr) noexcept :
			m_ptr(ptr)
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
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			other.m_ptr = nullptr;
		}

		~UniquePtr() noexcept
		{
			decOwnerCount();
		}

		UniquePtr& operator=(nullptr_t other) noexcept
		{
			decOwnerCount();
			m_ptr = nullptr;
			return *this;
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			if (this != std::addressof(other))
			{
				decOwnerCount();
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		template<typename T2, typename D2, std::enable_if_t<convertable_unique_ptr_v<T, T2>, int> = 0>
		UniquePtr& operator=(UniquePtr<T2, D2>&& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
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
		void assignRawPointer(pointer ptr)
		{
			decOwnerCount();
			m_ptr = ptr;
		}
	private:
		void incOwnerCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::IncOwnerCount(m_ptr->getAddress());
				}
				else
				{
					Box::IncOwnerCount(m_ptr);
				}
			}
		}
		void decOwnerCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::DecOwnerCount(m_ptr->getAddress());
				}
				else
				{
					Box::DecOwnerCount(m_ptr);
				}
			}
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
		template <typename T2>
		friend class BorrowedArray;

		template <typename T2, typename D2>
		friend class UniqueArray;

		template <typename T2, typename D2>
		friend class SharedArray;

		friend class Variant;

	public:
		using element_type = T;
		using pointer = T * ;
		using reference = T & ;
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
			PAF_ASSERT(0 == idx || idx < m_size);
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
		void assignRawPointer(pointer ptr, size_t size)
		{
			decOwnerCount();
			m_ptr = ptr;
			m_size = size;
		}
	private:
		void incOwnerCount()
		{
			if (m_ptr)
			{
				ArrayBox::IncOwnerCount(m_ptr);
			}
		}
		void decOwnerCount()
		{
			if (m_ptr)
			{
				ArrayBox::DecOwnerCount(m_ptr, m_size);
			}
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
		using pointer = T * ;
		using reference = T & ;
	public:
		constexpr BorrowedPtr() noexcept :
			m_ptr(nullptr)
		{}

		constexpr BorrowedPtr(nullptr_t) noexcept :
			m_ptr(nullptr)
		{}

		explicit BorrowedPtr(pointer ptr) noexcept :
			m_ptr(ptr)
		{
			incBorrowCount();
		}

		BorrowedPtr(const BorrowedPtr& other) noexcept :
			m_ptr(other.m_ptr)
		{
			incBorrowCount();
		}

		template<typename T2>
		BorrowedPtr(const BorrowedPtr<T2>& other) noexcept :
			m_ptr(static_cast<T*>(other.get()))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incBorrowCount();
		}

		template<typename T2, typename D2>
		explicit BorrowedPtr(const UniquePtr<T2, D2>& other) noexcept :
			m_ptr(static_cast<T*>(other.get()))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incBorrowCount();
		}

		template<typename T2, typename D2>
		explicit BorrowedPtr(const SharedPtr<T2, D2>& other) noexcept :
			m_ptr(static_cast<T*>(other.get()))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incBorrowCount();
		}

		BorrowedPtr& operator=(nullptr_t other) noexcept
		{
			decBorrowCount();
			m_ptr = nullptr;
			return *this;
		}

		BorrowedPtr& operator=(pointer ptr) noexcept
		{
			reset(ptr);
			return *this;
		}

		BorrowedPtr& operator=(const BorrowedPtr& other) noexcept
		{
			if (this != std::addressof(other))
			{
				reset(other.get());
			}
			return *this;
		}

		template<typename T2>
		BorrowedPtr& operator=(const BorrowedPtr<T2>& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			reset(other.get());
			return *this;
		}

		template<typename T2, typename D2>
		BorrowedPtr& operator=(const UniquePtr<T2, D2>& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			reset(other.get());
			return *this;
		}

		template<typename T2, typename D2>
		BorrowedPtr& operator=(const SharedPtr<T2, D2>& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			reset(other.get());
			return *this;
		}

		reference operator*() const noexcept
		{
			PAF_ASSERT(m_ptr);
			PAF_ASSERT(!isDangling());
			return *m_ptr;
		}

		pointer operator->() const noexcept
		{
			PAF_ASSERT(m_ptr);
			PAF_ASSERT(!isDangling());
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
			PAF_ASSERT(!isDangling());
			return m_ptr;
		}
	private:
		void assignRawPointer(pointer ptr)
		{
			decBorrowCount();
			m_ptr = ptr;
			incBorrowCount();
		}
	private:
		void incBorrowCount()
		{
#if _DEBUG
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::IncBorrowCount(m_ptr->getAddress());
				}
				else
				{
					Box::IncBorrowCount(m_ptr);
				}
			}
#endif
		}
		void decBorrowCount()
		{
#if _DEBUG
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::DecBorrowCount(m_ptr->getAddress());
				}
				else
				{
					Box::DecBorrowCount(m_ptr);
				}
			}
#endif
		}
		void reset(T* ptr)
		{
			decBorrowCount();
			m_ptr = ptr;
			incBorrowCount();
		}
		bool isDangling() const 
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
		using pointer = T * ;
		using reference = T & ;
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
				decBorrowCount();
				m_ptr = other.get();
				m_size = other.size();
				incBorrowCount();
			}
			return *this;
		}

		BorrowedArray& operator=(const UniqueArray<T>&other) noexcept
		{
			decBorrowCount();
			m_ptr = other.get();
			m_size = other.size();
			incBorrowCount();
			return *this;
		}

		BorrowedArray& operator=(const SharedArray<T>&other) noexcept
		{
			decBorrowCount();
			m_ptr = other.get();
			m_size = other.size();
			incBorrowCount();
			return *this;
		}

		reference operator[](size_t idx) const noexcept
		{
			PAF_ASSERT(m_ptr && idx < m_size);
			PAF_ASSERT(!isDangling());
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
			PAF_ASSERT(!isDangling());
			return m_ptr;
		}

		pointer get(size_t idx) const noexcept
		{
			PAF_ASSERT(0 == idx || idx < m_size);
			PAF_ASSERT(!isDangling());
			return idx < m_size ? m_ptr + idx : nullptr;
		}

		size_type size() const noexcept
		{
			return m_size;
		}
	private:
		void assignRawPointer(pointer ptr, size_t size)
		{
			decBorrowCount();
			m_ptr = ptr;
			m_size = size;
			incBorrowCount();
		}
	private:
		void incBorrowCount()
		{
#if _DEBUG
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					ArrayBox::IncBorrowCount(m_ptr->getAddress());
				}
				else
				{
					ArrayBox::IncBorrowCount(m_ptr);
				}
			}
#endif
		}
		void decBorrowCount()
		{
#if _DEBUG
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					ArrayBox::DecBorrowCount(m_ptr->getAddress());
				}
				else
				{
					ArrayBox::DecBorrowCount(m_ptr);
				}
			}
#endif
		}
	private:
		T* m_ptr;
		size_t m_size;
	};


	template<typename T>
	class RawPtr
	{
		template <typename T2>
		friend class RawPtr;

		template <typename T2>
		friend class BorrowedPtr;

		template <typename T2, typename D2>
		friend class UniquePtr;

		template<typename T2, typename D2>
		friend class SharedPtr;

		friend class Variant;

	public:
		using element_type = T;
		using pointer = T * ;
		using reference = T & ;
	public:
		constexpr RawPtr() noexcept :
			m_ptr(nullptr)
		{}

		constexpr RawPtr(nullptr_t) noexcept :
			m_ptr(nullptr)
		{}

		RawPtr(pointer ptr) noexcept :
			m_ptr(ptr)
		{}

		RawPtr(const RawPtr& other) noexcept :
			m_ptr(other.m_ptr)
		{}

		template<typename T2>
		RawPtr(const RawPtr<T2>& other) noexcept :
			m_ptr(other.m_ptr)
		{}

		template<typename T2>
		RawPtr(const BorrowedPtr<T2>& other) noexcept :
			m_ptr(other.m_ptr)
		{}

		template<typename T2, typename D2>
		explicit RawPtr(const UniquePtr<T2, D2>& other) noexcept :
			m_ptr(other.m_ptr)
		{}

		template<typename T2, typename D2>
		explicit RawPtr(const SharedPtr<T2, D2>& other) noexcept :
			m_ptr(other.m_ptr)
		{}

		RawPtr& operator=(nullptr_t other) noexcept
		{
			m_ptr = nullptr;
			return *this;
		}

		RawPtr& operator=(const RawPtr& other) noexcept
		{
			m_ptr = other.m_ptr;
			return *this;
		}

		template<typename T2>
		RawPtr& operator=(const BorrowedPtr<T2>& other) noexcept
		{
			m_ptr = other.m_ptr;
			return *this;
		}

		template<typename T2, typename D2>
		RawPtr& operator=(const UniquePtr<T2, D2>& other) noexcept
		{
			m_ptr = other.m_ptr;
			return *this;
		}

		template<typename T2, typename D2>
		RawPtr& operator=(const SharedPtr<T2, D2>& other) noexcept
		{
			m_ptr = other.m_ptr;
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
		bool operator==(const RawPtr<T2>& other) const
		{
			return m_ptr == other.m_ptr;
		}

		template<typename T2>
		bool operator!=(const RawPtr<T2>& other) const
		{
			return m_ptr != other.m_ptr;
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

		operator const T*() const
		{
			return  m_ptr;
		}

		operator T*() const
		{
			return  m_ptr;
		}

		pointer get() const noexcept
		{
			return m_ptr;
		}

		void assignRawPointer(pointer ptr)
		{
			m_ptr = ptr;
		}
	private:
		T* m_ptr;
	};


	template<typename T>
	class RawArray
	{
		template <typename T2>
		friend class RawArray;

		template <typename T2>
		friend class BorrowedArray;

		template <typename T2, typename D2>
		friend class UniqueArray;

		template<typename T2, typename D2>
		friend class SharedArray;

		friend class Variant;

	public:
		using element_type = T;
		using pointer = T * ;
		using reference = T & ;
		using size_type = size_t;
	public:
		constexpr RawArray() noexcept :
			m_ptr(nullptr),
			m_size(0)
		{}

		RawArray(const RawArray& other) noexcept :
			m_ptr(other.get()),
			m_size(other.size())
		{}

		RawArray(RawArray&& other) noexcept :
			m_ptr(other.get()),
			m_size(other.size())
		{
			other.m_ptr = nullptr;
			other.m_size = 0;
		}

		explicit RawArray(const UniqueArray<T>&other) noexcept :
			m_ptr(other.get()),
			m_size(other.size())
		{}

		explicit RawArray(const SharedArray<T>&other) noexcept :
			m_ptr(other.get()),
			m_size(other.size())
		{}

		RawArray& operator=(nullptr_t other) noexcept
		{
			m_ptr = nullptr;
			m_size = 0;
			return *this;
		}

		RawArray& operator=(const RawArray& other) noexcept
		{
			m_ptr = other.get();
			m_size = other.size();
			return *this;
		}

		RawArray& operator=(const BorrowedArray<T> & other) noexcept
		{
			m_ptr = other.get();
			m_size = other.size();
			return *this;
		}

		RawArray& operator=(const UniqueArray<T>&other) noexcept
		{
			m_ptr = other.get();
			m_size = other.size();
			return *this;
		}

		RawArray& operator=(const SharedArray<T>&other) noexcept
		{
			m_ptr = other.get();
			m_size = other.size();
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

		bool operator==(const T * ptr) const
		{
			return m_ptr == ptr;
		}

		bool operator!=(const T * ptr) const
		{
			return m_ptr != ptr;
		}

		bool operator==(const RawArray & other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const RawArray & other) const
		{
			return m_ptr != other.m_ptr;
		}

		bool operator==(const BorrowedArray<T> & other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const BorrowedArray<T> & other) const
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
			return m_ptr;
		}

		pointer get(size_t idx) const noexcept
		{
			PAF_ASSERT(0 == idx || idx < m_size);
			return idx < m_size ? m_ptr + idx : nullptr;
		}

		size_type size() const noexcept
		{
			return m_size;
		}

		void assignRawPointer(pointer ptr, size_t size)
		{
			m_ptr = ptr;
			m_size = size;
		}
	private:
		T* m_ptr;
		size_t m_size;
	};

}
