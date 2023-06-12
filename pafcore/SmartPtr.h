#pragma once

#include "Utility.h"
#include <atomic>
#include <type_traits>
#include <memory>

//std::shared_ptr<int>

namespace paf
{
	template<typename T>
	static constexpr bool is_interface = std::is_base_of_v<::paf::Interface, T>;

	template<typename T1, typename T2>
	static constexpr bool convertable_shared_ptr_v = (std::is_base_of_v<T1, T2> || std::is_base_of_v<T2, T1>) && std::has_virtual_destructor_v<T1> && std::has_virtual_destructor_v<T2>;

	class Box
	{
	public:
		virtual void destruct() = 0;
		virtual void deallocate() = 0;
	public:
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
				decWeakCount();
			}
		}
		bool dangling() const
		{
			return 0 == m_strongCount;
		}
	protected:
		mutable long m_strongCount{ 1 };
		mutable long m_weakCount{ 1 };
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

		static void IncWeakCount(const void* ptr)
		{
			FromRawPtr(ptr)->incWeakCount();
		}

		static void DecWeakCount(const void* ptr)
		{
			FromRawPtr(ptr)->decWeakCount();
		}

		static void IncStrongCount(const void* ptr)
		{
			FromRawPtr(ptr)->incStrongCount();
		}

		static void DecStrongCount(const void* ptr)
		{
			FromRawPtr(ptr)->decStrongCount();
		}
	};

	class ArrayBox
	{
	public:
		virtual void destructArray(size_t arraySize) = 0;
		virtual void deallocate() = 0;
	public:
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
		void incStrongCount()
		{
			PAF_ASSERT(!dangling());
			++m_strongCount;
		}
		void decStrongCount(size_t arraySize)
		{
			if (0 == --m_strongCount)
			{
				destructArray(arraySize);
				decWeakCount();
			}
		}
		bool dangling() const
		{
			return 0 == m_strongCount;
		}
	protected:
		mutable long m_strongCount{ 1 };
		mutable long m_weakCount{ 1 };
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

		static void IncWeakCount(const void* ptr)
		{
			FromRawPtr(ptr)->incWeakCount();
		}

		static void DecWeakCount(const void* ptr)
		{
			FromRawPtr(ptr)->decWeakCount();
		}

		static void IncStrongCount(const void* ptr)
		{
			FromRawPtr(ptr)->incStrongCount();
		}

		static void DecStrongCount(const void* ptr, size_t arraySize)
		{
			FromRawPtr(ptr)->decStrongCount(arraySize);
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
	class SharedPtr;

	template<typename T, typename D = Destructor<T>>
	class SharedArray;

	template<typename T>
	class WeakPtr;

	template<typename T>
	class WeakArray;

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
		friend class WeakPtr;

		template<typename T2, typename D2>
		friend class SharedPtr;

		friend class Variant;

	public:
		using pointer = T * ;
		using reference = T & ;
	private:
		SharedPtr(pointer ptr) noexcept :
			m_ptr(ptr)
		{}
		//for shared from other raw pointer
		SharedPtr(pointer ptr, std::true_type tag) noexcept :
			m_ptr(ptr)
		{
			incStrongCount();
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
			incStrongCount();
		}

		template <typename T2, typename D2, std::enable_if_t<convertable_shared_ptr_v<T, T2>, int> = 0>
		SharedPtr(const SharedPtr<T2, D2>& other) noexcept :
			m_ptr(static_cast<T*>(other.m_ptr))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incStrongCount();
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

		operator pointer() const noexcept
		{
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
		void incStrongCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::IncStrongCount(m_ptr->getMemoryAddress());
				}
				else
				{
					Box::IncStrongCount(m_ptr);
				}
			}
		}
		void decStrongCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::DecStrongCount(m_ptr->getMemoryAddress());
				}
				else
				{
					Box::DecStrongCount(m_ptr);
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
			return SharedPtr(ptr, std::true_type());
		}
	};


	template<typename T, typename D>
	class SharedArray
	{
		template <typename T2>
		friend class WeakArray;

		template<typename T2, typename D2>
		friend class SharedArray;

		friend class Variant;

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
			incStrongCount();
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

		bool operator==(const WeakArray<T>& other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const WeakArray<T>& other) const
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
			ArrayBox::IncStrongCount(ptr);
			ArrayBox::DecStrongCount(m_ptr, m_size);
			m_ptr = ptr;
			m_size = size;
		}
	private:
		void incStrongCount()
		{
			if (m_ptr)
			{
				ArrayBox::IncStrongCount(m_ptr);
			}
		}
		void decStrongCount()
		{
			if (m_ptr)
			{
				ArrayBox::DecStrongCount(m_ptr, m_size);
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



	template<typename T>
	class WeakPtr
	{
		template <typename T2>
		friend class WeakPtr;

		template<typename T2, typename D2>
		friend class SharedPtr;

		friend class Variant;

	public:
		using element_type = T;
		using pointer = T * ;
		using reference = T & ;
	public:
		constexpr WeakPtr() noexcept :
			m_ptr(nullptr)
		{}

		constexpr WeakPtr(nullptr_t) noexcept :
			m_ptr(nullptr)
		{}

		explicit WeakPtr(pointer ptr) noexcept :
			m_ptr(ptr)
		{
			incWeakCount();
		}

		WeakPtr(const WeakPtr& other) noexcept :
			m_ptr(other.m_ptr)
		{
			incWeakCount();
		}

		template<typename T2>
		WeakPtr(const WeakPtr<T2>& other) noexcept :
			m_ptr(static_cast<T*>(other.get()))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incWeakCount();
		}

		template<typename T2, typename D2>
		explicit WeakPtr(const SharedPtr<T2, D2>& other) noexcept :
			m_ptr(static_cast<T*>(other.get()))
		{
			PAF_ASSERT(is_interface<T> || paf_base_offset_of(T, T2) == 0);
			incWeakCount();
		}

		WeakPtr& operator=(nullptr_t other) noexcept
		{
			decWeakCount();
			m_ptr = nullptr;
			return *this;
		}

		WeakPtr& operator=(pointer ptr) noexcept
		{
			reset(ptr);
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
			return m_ptr;
		}
	private:
		void assignRawPointer(pointer ptr)
		{
			decWeakCount();
			m_ptr = ptr;
			incWeakCount();
		}
	private:
		void incWeakCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::IncWeakCount(m_ptr->getMemoryAddress());
				}
				else
				{
					Box::IncWeakCount(m_ptr);
				}
			}
		}
		void decWeakCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					Box::DecWeakCount(m_ptr->getMemoryAddress());
				}
				else
				{
					Box::DecWeakCount(m_ptr);
				}
			}
		}
		void reset(T* ptr)
		{
			decWeakCount();
			m_ptr = ptr;
			incWeakCount();
		}
		bool isDangling() const 
		{
			return Box::IsDangling(m_ptr);
		}
	private:
		T* m_ptr;
	};


	template<typename T>
	class WeakArray
	{
		template <typename T2>
		friend class WeakArray;

		template<typename T2, typename D2>
		friend class SharedArray;

		friend class Variant;

	public:
		using element_type = T;
		using pointer = T * ;
		using reference = T & ;
		using size_type = size_t;
	public:
		constexpr WeakArray() noexcept :
			m_ptr(nullptr),
			m_size(0)
		{}

		WeakArray(const WeakArray& other) noexcept :
			m_ptr(other.get()),
			m_size(other.size())
		{
			incWeakCount();
		}

		WeakArray(WeakArray&& other) noexcept :
			m_ptr(other.get()),
			m_size(other.size())
		{
			other.m_ptr = nullptr;
			other.m_size = 0;
		}

		explicit WeakArray(const SharedArray<T>&other) noexcept :
			m_ptr(other.get()),
			m_size(other.size())
		{
			incWeakCount();
		}

		WeakArray& operator=(nullptr_t other) noexcept
		{
			decWeakCount();
			m_ptr = nullptr;
			m_size = 0;
			return *this;
		}

		WeakArray& operator=(const WeakArray & other) noexcept
		{
			if (this != std::addressof(other))
			{
				decWeakCount();
				m_ptr = other.get();
				m_size = other.size();
				incWeakCount();
			}
			return *this;
		}

		WeakArray& operator=(const SharedArray<T>&other) noexcept
		{
			decWeakCount();
			m_ptr = other.get();
			m_size = other.size();
			incWeakCount();
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

		bool operator==(const WeakArray & other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const WeakArray & other) const
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
			decWeakCount();
			m_ptr = ptr;
			m_size = size;
			incWeakCount();
		}
	private:
		void incWeakCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					ArrayBox::IncWeakCount(m_ptr->getMemoryAddress());
				}
				else
				{
					ArrayBox::IncWeakCount(m_ptr);
				}
			}
		}
		void decWeakCount()
		{
			if (m_ptr)
			{
				if constexpr (is_interface<T>)
				{
					ArrayBox::DecWeakCount(m_ptr->getMemoryAddress());
				}
				else
				{
					ArrayBox::DecWeakCount(m_ptr);
				}
			}
		}
	private:
		T* m_ptr;
		size_t m_size;
	};

}
