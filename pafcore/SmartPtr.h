#pragma once

#include "Utility.h"
#include <atomic>
#include <type_traits>
#include <memory>
//#include <array>
//std::shared_ptr<int[]>
//std::array<int, 5>
//std::unique_ptr<int[]>;

namespace paf
{
	template<typename T>
	static constexpr bool is_interface = std::is_base_of_v<::paf::Interface, T>;

	template<typename T1, typename T2>
	static constexpr bool convertable_shared_ptr_v = (std::is_base_of_v<T1, T2> || std::is_base_of_v<T2, T1>) && std::has_virtual_destructor_v<T1> && std::has_virtual_destructor_v<T2>;

	class RefCountBase
	{
	public:
		virtual void destruct() = 0;
		virtual void deallocate() = 0;
	public:
		RefCountBase() : m_weakCount(1)
		{}
		RefCountBase(uint32_t arraySize) : m_arraySize(arraySize)
		{}
#ifdef _DEBUG
		bool dangling() const
		{
			return 0 == m_strongCount;
		}
		void incWeakCount()
		{
			PAF_ASSERT(!dangling());
			++m_weakCount;
		}
		void decWeakCount()
		{
			PAF_ASSERT(0 < m_weakCount);
			if (0 == --m_weakCount)
			{
				PAF_ASSERT(0 == m_strongCount);
				deallocate();
			}
		}
		static bool IsDangling(const void* ptr)
		{
			return ptr ? static_cast<const RefCountBase*>(ptr)->dangling() : false;
		}

		static void IncWeakCount(const void* ptr)
		{
			FromRawPtr(ptr)->incWeakCount();
		}

		static void DecWeakCount(const void* ptr)
		{
			FromRawPtr(ptr)->decWeakCount();
		}
#endif
		void incStrongCount()
		{
			PAF_ASSERT(!dangling());
			++m_strongCount;
		}
		void decStrongCount()
		{
			if (0 == --m_strongCount)
			{
				destruct();
#ifdef _DEBUG
				decWeakCount();
#else
				deallocate();
#endif
			}
		}
		void decStrongCountForArray()
		{
			if (0 == --m_strongCount)
			{
				destruct();
				deallocate();
			}
		}

	protected:
		mutable long m_strongCount{ 1 };
		union
		{
			mutable long m_arraySize;
			mutable long m_weakCount;
		};
	public:
		static RefCountBase* FromRawPtr(const void* ptr)
		{
			PAF_ASSERT(ptr);
			return (RefCountBase*)(ptr)-1;
		}
		static void IncStrongCount(const void* ptr)
		{
			FromRawPtr(ptr)->incStrongCount();
		}
		static void DecStrongCount(const void* ptr)
		{
			FromRawPtr(ptr)->decStrongCount();
		}
		static void DecStrongCountForArray(const void* ptr)
		{
			FromRawPtr(ptr)->decStrongCountForArray();
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
		static void DestructArray(T* ptr, uint32_t arraySize)
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
	class RefCountImpl : public RefCountBase
	{
		RefCountImpl() = default;
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
			static_assert(sizeof(RefCountBase) == sizeof(RefCountImpl));
			size_t size = sizeof(RefCountImpl) + sizeof(T);
			void* p = malloc(size);
			RefCountImpl* box = (RefCountImpl*)p;
			new(box)RefCountImpl();
			T* ptr = (T*)(box + 1);
			new(ptr)T(std::forward<Types>(args)...);
			return ptr;
		}
	};

	template<typename T, typename D = Destructor<T>>
	class ArrayRefCountImpl : public RefCountBase
	{
	public:
		ArrayRefCountImpl(uint32_t arraySize) :
			RefCountBase(arraySize)
		{}
		void destruct() override
		{
			D::DestructArray(reinterpret_cast<T*>(this + 1), this->m_arraySize);
		}
		void deallocate() override
		{
			free(this);
		}
	public:
		static T* NewArray(uint32_t arraySize)
		{
			static_assert(sizeof(RefCountBase) == sizeof(ArrayRefCountImpl));
			size_t size = sizeof(ArrayRefCountImpl) + sizeof(T) * arraySize;
			void* p = malloc(size);
			ArrayRefCountImpl* box = (ArrayRefCountImpl*)p;
			new(box)ArrayRefCountImpl(arraySize);
			T* ptr = (T*)(box + 1);
			new(ptr)T[arraySize];
			return ptr;
		}
	};

	class Variant;

	class RefPtrBase
	{
		friend class Variant;
	public:
		void* ptr() const
		{
			return m_ptr;
		}
	protected:
		RefPtrBase(void* ptr) : m_ptr(ptr)
		{}
	protected:
		void* m_ptr;
	};

	template<typename T, typename D = Destructor<T>>
	class SharedPtr;

	template<typename T, typename D = Destructor<T>>
	class SharedArray;

	template<typename T>
	class WeakPtr;


	template<typename T, typename D>
	class SharedPtr : public RefPtrBase
	{
		template <typename T2>
		friend class WeakPtr;

		template<typename T2, typename D2>
		friend class SharedPtr;

	public:
		using pointer = T * ;
		using reference = T & ;
	private:
		SharedPtr(pointer ptr) noexcept :
			RefPtrBase(ptr)
		{}
		//for shared from other raw pointer
		SharedPtr(pointer ptr, std::true_type tag) noexcept :
			RefPtrBase(ptr)
		{
			incStrongCount();
		}
	public:
		constexpr SharedPtr() noexcept :
			RefPtrBase(nullptr)
		{}

		constexpr SharedPtr(nullptr_t) noexcept :
			RefPtrBase(nullptr)
		{}

		SharedPtr(const SharedPtr& other) noexcept :
			RefPtrBase(other.m_ptr)
		{
			incStrongCount();
		}

		template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
		SharedPtr(const SharedPtr<T2, D2>& other) noexcept :
			RefPtrBase(static_cast<T*>(other.m_ptr))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incStrongCount();
		}

		SharedPtr(SharedPtr&& other) noexcept :
			RefPtrBase(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
		SharedPtr(SharedPtr<T2, D2>&& other) noexcept :
			RefPtrBase(static_cast<T*>(other.m_ptr))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			other.m_ptr = nullptr;
		}

		~SharedPtr() noexcept
		{
			decStrongCount();
		}

		SharedPtr& operator=(nullptr_t) noexcept
		{
			decStrongCount();
			m_ptr = nullptr;
			return *this;
		}

		SharedPtr& operator=(pointer ptr) noexcept
		{
			assignRawPointer(ptr);
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
			decStrongCount();
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			return *this;
		}

		template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
		SharedPtr& operator=(SharedPtr<T2, D2>&& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			decStrongCount();
			m_ptr = static_cast<T*>(other.get());
			other.m_ptr = nullptr;
			return *this;
		}


		reference operator*() const noexcept
		{
			PAF_ASSERT(m_ptr);
			return *get();
		}

		pointer operator->() const noexcept
		{
			PAF_ASSERT(m_ptr);
			return get();
		}

		//operator pointer() const noexcept
		//{
		//	return m_ptr;
		//}
		
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
		bool operator==(const WeakPtr<T2>& other) const
		{
			return m_ptr == other.m_ptr;
		}

		template<typename T2>
		bool operator!=(const WeakPtr<T2>& other) const
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
			return reinterpret_cast<pointer>(m_ptr);
		}

	private:
		//for variant
		void assignRawPointer(pointer ptr)
		{
			SharedPtr(ptr, std::true_type()).swap(*this);
		}
		void swap(SharedPtr& other) noexcept
		{
			std::swap(m_ptr, other.m_ptr);
		}
	private:
		void incStrongCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					RefCountBase::IncStrongCount(get()->getMemoryAddress());
				}
				else
				{
					RefCountBase::IncStrongCount(m_ptr);
				}
			}
		}
		void decStrongCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					RefCountBase::DecStrongCount(get()->getMemoryAddress());
				}
				else
				{
					RefCountBase::DecStrongCount(m_ptr);
				}
			}
		}
	public:
		template <typename... Types>
		static SharedPtr Make(Types&&... args)
		{
			T* p = RefCountImpl<T, D>::New(std::forward<Types>(args)...);
			return SharedPtr(p);
		}
		static SharedPtr SharedFrom(T* ptr)
		{
			return SharedPtr(ptr, std::true_type());
		}
	};


	template<typename T, typename D>
	class SharedArray : public RefPtrBase
	{
	public:
		using pointer = T * ;
		using reference = T & ;
		using size_type = size_t;
	private:
		SharedArray(pointer ptr) noexcept :
			RefPtrBase(ptr)
		{}
	public:
		constexpr SharedArray() noexcept :
			RefPtrBase(nullptr)
		{}

		constexpr SharedArray(nullptr_t) noexcept :
			RefPtrBase(nullptr)
		{}

		SharedArray(SharedArray const& other) noexcept :
			RefPtrBase(other.m_ptr)
		{
			incStrongCount();
		}

		SharedArray(SharedArray&& other) noexcept :
			RefPtrBase(other.m_ptr)
		{
			other.m_ptr = nullptr;
		}

		~SharedArray() noexcept
		{
			decStrongCount();
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

		//reference operator[](size_t idx) const noexcept
		//{
		//	PAF_ASSERT(m_ptr && idx < m_size);
		//	return m_ptr[idx];
		//}

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

		bool operator==(const SharedArray& other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const SharedArray& other) const
		{
			return m_ptr != other.m_ptr;
		}

		pointer get() const noexcept
		{
			return m_ptr;
		}

		//pointer get(size_t idx) const noexcept
		//{
		//	PAF_ASSERT(0 == idx || idx < m_size);
		//	return idx < m_size ? m_ptr + idx : nullptr;
		//}

		//size_type size() const noexcept
		//{
		//	return m_size;
		//}

	private:
		//for variant
		//void assignRawPointer(pointer ptr, size_t size)
		//{
		//	ArrayRefCountBase::IncStrongCount(ptr);
		//	ArrayRefCountBase::DecStrongCount(m_ptr, m_size);
		//	m_ptr = ptr;
		//	m_size = size;
		//}
		void swap(SharedArray& other) noexcept
		{
			std::swap(m_ptr, other.m_ptr);
			std::swap(m_size, other.m_size);
		}
	private:
		void incStrongCount()
		{
			if (m_ptr)
			{
				RefCountBase::IncStrongCount(m_ptr);
			}
		}
		void decStrongCount()
		{
			if (m_ptr)
			{
				RefCountBase::DecStrongCountForArray(m_ptr);
			}
		}
	public:
		static SharedArray Make(const uint32_t arraySize)
		{
			T* p = ArrayRefCountImpl<T, D>::NewArray(arraySize);
			return SharedArray(p);
		}
	};


	template<typename T>
	class WeakPtr : public RefPtrBase
	{
		template <typename T2>
		friend class WeakPtr;

		template<typename T2, typename D2>
		friend class SharedPtr;

		friend class Variant;

	public:
		using element_type = T;
		using pointer = T * ;
		using reference = T &;
	private:
		explicit WeakPtr(pointer ptr) noexcept :
			RefPtrBase(ptr)
		{
			incWeakCount();
		}
	public:
		constexpr WeakPtr() noexcept :
			RefPtrBase(nullptr)
		{}

		constexpr WeakPtr(nullptr_t) noexcept :
			RefPtrBase(nullptr)
		{}

		WeakPtr(const WeakPtr& other) noexcept :
			RefPtrBase(other.m_ptr)
		{
			incWeakCount();
		}

		template<typename T2>
		WeakPtr(const WeakPtr<T2>& other) noexcept :
			RefPtrBase(static_cast<T*>(other.get()))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incWeakCount();
		}

		template<typename T2, typename D2>
		explicit WeakPtr(const SharedPtr<T2, D2>& other) noexcept :
			RefPtrBase(static_cast<T*>(other.get()))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incWeakCount();
		}

		~WeakPtr() noexcept
		{
			decWeakCount();
		}

		WeakPtr& operator=(nullptr_t other) noexcept
		{
			decWeakCount();
			m_ptr = nullptr;
			return *this;
		}

		WeakPtr& operator=(pointer ptr) noexcept
		{
			WeakPtr(ptr).swap(*this);
			return *this;
		}

		WeakPtr& operator=(const WeakPtr& other) noexcept
		{
			if (this != std::addressof(other))
			{
				reset(other.get());
			}
			return *this;
		}

		template<typename T2>
		WeakPtr& operator=(const WeakPtr<T2>& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			reset(other.get());
			return *this;
		}

		template<typename T2, typename D2>
		WeakPtr& operator=(const SharedPtr<T2, D2>& other) noexcept
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			reset(other.get());
			return *this;
		}

		reference operator*() const noexcept
		{
			PAF_ASSERT(m_ptr);
			PAF_ASSERT(!isDangling());
			return *get();
		}

		pointer operator->() const noexcept
		{
			PAF_ASSERT(m_ptr);
			PAF_ASSERT(!isDangling());
			return get();
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
		bool operator==(const WeakPtr<T2>& other) const
		{
			return m_ptr == other.m_ptr;
		}

		template<typename T2>
		bool operator!=(const WeakPtr<T2>& other) const
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
			return (pointer)m_ptr;
		}


	private:
		//void assignRawPointer(pointer ptr)
		//{
		//	decWeakCount();
		//	m_ptr = ptr;
		//	incWeakCount();
		//}
		void swap(WeakPtr& other) noexcept
		{
			std::swap(m_ptr, other.m_ptr);
		}
	private:
		void incWeakCount()
		{
#ifdef _DEBUG
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					RefCountBase::IncWeakCount(get()->getMemoryAddress());
				}
				else
				{
					RefCountBase::IncWeakCount(m_ptr);
				}
			}
#endif
		}
		void decWeakCount()
		{
#ifdef _DEBUG
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					RefCountBase::DecWeakCount(get()->getMemoryAddress());
				}
				else
				{
					RefCountBase::DecWeakCount(m_ptr);
				}
			}
#endif
		}
		void reset(T* ptr)
		{
			decWeakCount();
			m_ptr = ptr;
			incWeakCount();
		}
#ifdef _DEBUG
		bool isDangling() const 
		{
			return RefCountBase::IsDangling(m_ptr);
		}
#endif
	};

}
