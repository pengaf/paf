#{
#include <assert.h>
#include <type_traits>

#if defined PAFCORE_EXPORTS
	#define PAFCORE_EXPORT __declspec(dllexport)
	#define PAFCORE_TEMPLATE __declspec(dllexport)
#else
	#define PAFCORE_EXPORT __declspec(dllimport)
	#define PAFCORE_TEMPLATE
#endif

#if _DEBUG
#define PAF_DEBUG_ONLY(...) __VA_ARGS__
#define PAF_RELEASE_ONLY(...)
#else
#define PAF_DEBUG_ONLY(...)
#define PAF_RELEASE_ONLY(...) __VA_ARGS__
#endif

#define BEGIN_PAF namespace paf {
#define END_PAF }

#define PAF_CONCAT_(a, b) a ## b
#define PAF_CONCAT(a, b)  PAF_CONCAT_(a, b)

#define PAF_WIDE_(s) L ## s
#define PAF_WIDE(s) PAF_WIDE_(s)

#ifdef _DEBUG
#define paf_new new(__FILE__, __LINE__, 0)


PAFCORE_EXPORT void PafAssert(wchar_t const* _Message, wchar_t const* _File, unsigned _Line, wchar_t const* format, ...);

#define PAF_ASSERT_MSG(expression, format, ...) (void)(			\
		(!!(expression)) ||							\
		(PafAssert(PAF_WIDE(#expression), PAF_WIDE(__FILE__), (unsigned)(__LINE__), PAF_WIDE(format), __VA_ARGS__), 0) \
	)

#else

#define paf_new new
#define PAF_ASSERT_MSG(expression, format, ...) ((void)0)

#endif

#define PAF_ASSERT_MSG0(expression, msg)  PAF_ASSERT_MSG(expression, "%s", msg)
#define PAF_ASSERT(expression)  assert(expression)

#define paf_array_size_of(a)	(sizeof(a)/sizeof(a[0]))
#define paf_field_size_of(s, m)	sizeof(((s*)0)->m)
#define paf_field_array_size_of(s, m)	(sizeof(((s*)0)->m)/sizeof(((s*)0)->m[0]))
#define paf_base_offset_of(d, b) (reinterpret_cast<ptrdiff_t>(static_cast<b*>(reinterpret_cast<d*>(0x10000000))) - 0x10000000)
#define paf_verify

typedef bool					bool_t;
typedef char					char_t;
typedef signed char				schar_t;
typedef unsigned char			uchar_t;
typedef short					short_t;
typedef unsigned short			ushort_t;
typedef long					long_t;
typedef unsigned long			ulong_t;
typedef long long				longlong_t;
typedef unsigned long long		ulonglong_t;
typedef int						int_t;
typedef unsigned int			uint_t;
typedef float					float_t;
typedef double					double_t;
typedef long double				longdouble_t;

typedef unsigned char			byte_t;
typedef signed char				int8_t;
typedef unsigned char			uint8_t;
typedef short					int16_t;
typedef unsigned short			uint16_t;
typedef int						int32_t;
typedef unsigned int			uint32_t;
typedef long long				int64_t;
typedef unsigned long long		uint64_t;

template<typename T>
struct RuntimeTypeOf
{};


template<typename T>
struct AutoRegisterType
{
};

#define AUTO_REGISTER_TYPE(T)						\
template<>											\
struct AutoRegisterType<T>							\
{													\
	AutoRegisterType()								\
	{												\
		T::GetSingleton();							\
	}												\
	static AutoRegisterType<T> s_instance;			\
};													\
AutoRegisterType<T> AutoRegisterType<T>::s_instance;


template<typename T>
struct AutoRegisterTypeAlias
{
public:
	AutoRegisterTypeAlias()
	{
		T::GetSingleton();
	}
};

#define AUTO_REGISTER_TYPEALIAS(T) AutoRegisterTypeAlias<T> g_auto_register_##T;

#define BEGIN_AUTO_RUN(T)		\
struct AutoRun_##T				\
{								\
	AutoRun_##T()				\
	{

#define END_AUTO_RUN(T)			\
	}							\
};								\
static AutoRun_##T s_autoRun_##T;

#}

typename bool_t;
typename char_t;
typename schar_t;
typename uchar_t;
typename short_t;
typename ushort_t;
typename long_t;
typename ulong_t;
typename longlong_t;
typename ulonglong_t;
typename int_t;
typename uint_t;
typename float_t;
typename double_t;
typename longdouble_t;
typename byte_t;
typename int8_t;
typename uint8_t;
typename int16_t;
typename uint16_t;
typename int32_t;
typename uint32_t;
typename int64_t;
typename uint64_t;
typename size_t;
typename ptrdiff_t;

namespace paf
{
#{
	template<typename T, bool b = std::is_default_constructible_v<T>>
	struct PlacementNewDefaultCaller
	{
		static bool Call(void* p)
		{
			new(p)T;
			return true;
		}
	};
	template<typename T>
	struct PlacementNewDefaultCaller<T, false>
	{
		static bool Call(void* p)
		{
			return false;
		}
	};

	template<typename T, bool b = std::is_copy_constructible_v<T>>
	struct PlacementNewCopyCaller
	{
		static bool Call(void* p, const void* other)
		{
			new(p)T(*reinterpret_cast<const T*>(other));
			return true;
		}
	};
	template<typename T>
	struct PlacementNewCopyCaller<T, false>
	{
		static bool Call(void* p, const void* other)
		{
			return false;
		}
	};

	template<typename T, bool b = std::is_default_constructible_v<T>>
	struct PlacementNewArrayCaller
	{
		static bool Call(void* p, size_t count)
		{
			new(p)T[count];
			return true;
		}
	};
	template<typename T>
	struct PlacementNewArrayCaller<T, false>
	{
		static bool Call(void* p, size_t count)
		{
			return false;
		}
	};

	template<typename T, bool b = std::is_destructible_v<T>>
	struct DestructorCaller
	{
		static bool Call(void* p)
		{
			reinterpret_cast<T*>(p)->~T();
			return true;
		}
	};

	template<typename T>
	struct DestructorCaller<T, false>
	{
		static bool Call(void* p)
		{
			return false;
		}
	};

	template<typename T, bool b = std::is_copy_assignable_v<T>>
	struct CopyAssignmentCaller
	{
		static bool Call(void* dst, const void* src)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(src);
			return true;
		}
	};

	template<typename T>
	struct CopyAssignmentCaller<T, false>
	{
		static bool Call(void* dst, const void* src)
		{
			return false;
		}
	};

	template<typename T, typename U, bool b = std::is_assignable_v<T, U>>
	struct AssignmentCaller
	{
		static bool Call(void* dst, const void* src)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const U*>(src);
			return true;
		}
	};

	template<typename T, typename U>
	struct AssignmentCaller<T, U, false>
	{
		static bool Call(void* dst, const void* src)
		{
			return false;
		}
	};

	template<typename T, typename U, bool b = std::is_convertible_v<U, T>>
	struct CastCaller
	{
		static bool Call(void* dst, const void* src)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const U*>(src);
			return true;
		}
	};

	template<typename T, typename U>
	struct CastCaller<T, U, false>
	{
		static bool Call(void* dst, const void* src)
		{
			return false;
		}
	};

	template<typename T>
	class array_t
	{
	public:
		array_t() noexcept : m_ptr(nullptr), m_size(0)
		{}
		array_t(T* ptr, size_t size) noexcept : m_ptr(ptr), m_size(size)
		{}
	public:
		void assign(T* ptr, size_t size)
		{
			m_ptr = ptr;
			m_size = size;
		}
		T* get() const noexcept
		{
			return m_ptr;
		}
		size_t size() const noexcept
		{
			return m_size;
		}
		T& operator[](size_t idx) const noexcept
		{
			PAF_ASSERT(m_ptr && idx < m_size);
			return m_ptr[idx];
		}
		explicit operator bool() const noexcept
		{
			return static_cast<bool>(m_ptr);
		}
	private:
		T* m_ptr;
		size_t m_size;
	};

	const size_t max_method_param_count = 32;

	enum class ErrorCode
	{
		s_ok,
		e_name_conflict,
		e_is_not_type,
		e_is_not_class,
		e_is_not_string,
		e_is_not_array,
		e_invalid_subscript_type,
		e_member_not_found,
		e_index_out_of_range,
		e_is_not_property,
		e_is_not_array_property,
		e_is_not_collection_property,
		e_property_get_not_implemented,
		e_property_set_not_implemented,
		e_property_size_not_implemented,
		e_property_iterate_not_implemented,
		e_field_is_an_array,
		e_invalid_type,
		e_invalid_object_type,
		e_invalid_field_type,
		e_invalid_property_type,
		e_invalid_this_type,
		e_invalid_arg_type_1,
		e_invalid_arg_type_2,
		e_invalid_arg_type_3,
		e_invalid_arg_type_4,
		e_invalid_arg_type_5,
		e_invalid_arg_type_6,
		e_invalid_arg_type_7,
		e_invalid_arg_type_8,
		e_invalid_arg_type_9,
		e_invalid_arg_type_10,
		e_invalid_arg_type_11,
		e_invalid_arg_type_12,
		e_invalid_arg_type_13,
		e_invalid_arg_type_14,
		e_invalid_arg_type_15,
		e_invalid_arg_type_16,
		e_invalid_arg_type_17,
		e_invalid_arg_type_18,
		e_invalid_arg_type_19,
		e_invalid_arg_type_20,
		e_invalid_arg_type_21,
		e_invalid_arg_type_22,
		e_invalid_arg_type_23,
		e_invalid_arg_type_24,
		e_invalid_arg_type_25,
		e_invalid_arg_type_26,
		e_invalid_arg_type_27,
		e_invalid_arg_type_28,
		e_invalid_arg_type_29,
		e_invalid_arg_type_30,
		e_invalid_arg_type_31,
		e_invalid_arg_type_32,
		e_too_few_arguments,
		e_too_many_arguments,
		e_not_implemented,
		e_script_error,
		e_script_dose_not_override,
	};

	PAFCORE_EXPORT const char* ErrorCodeToString(ErrorCode errorCode);

	class PAFCORE_EXPORT VirtualDestructor
	{
	public:
		virtual ~VirtualDestructor()
		{}
	};

	class PAFCORE_EXPORT Interface : public VirtualDestructor
	{
	public:
		virtual void* getAddress() = 0;
	};

	class Variant;
	class PAFCORE_EXPORT SubclassInvoker : public VirtualDestructor
	{
	public:
		virtual ErrorCode invoke(const char* name, Variant* result, Variant* self, Variant* args, size_t numArgs) = 0;
	};

	class ClassType;
	class PAFCORE_EXPORT IntrospectableInterface : public Interface
	{
	public:
		virtual ClassType* getType() = 0;
	};

#}

	class(string) #PAFCORE_EXPORT string_t #final
	{
#{
	public:
		string_t() : m_str("")
		{}
		string_t(const char* str) : m_str(str ? str : "")
		{}
	public:
		const char* c_str() const
		{
			return m_str;
		}
		operator const char* () const
		{
			return m_str;
		}
		bool empty() const
		{
			return (0 == *m_str);
		}
		void assign(const char* str)
		{
			m_str = str ? str : "";
		}
		//bool operator == (const char* str) const
		//{
		//	return 0 == strcmp(m_str, str);
		//}
		//bool operator != (const char* str) const
		//{
		//	return 0 != strcmp(m_str, str);
		//}
	protected:
		const char* m_str;
#}
	};

	class(buffer) #PAFCORE_EXPORT buffer_t #final
	{
#{
	public:
		buffer_t() :
			m_ptr(nullptr), m_size(0)
		{}
		buffer_t(void* ptr, size_t size) :
			m_ptr(ptr), m_size(size)
		{}
		void assign(void* ptr, size_t size)
		{
			m_ptr = ptr;
			m_size = size;
		}
		void* getPtr() const
		{
			return m_ptr;
		}
		size_t getSize() const
		{
			return m_size;
		}
	protected:
		void* m_ptr;
		size_t m_size;
#}
	};

	//StringBase 的派生类在序列化，反序列化以及复制时特殊处理，调用toString, fromString， 不再看其下 field 和 property
	class #PAFCORE_EXPORT StringBase
	{
#{
	public:
		virtual string_t toString() const = 0;
		virtual void fromString(string_t str) = 0;
#}
	};

	//StringBase 的派生类在序列化，反序列化以及复制时特殊处理，调用toBuffer, fromBuffer， 不再看其下 field 和 property
	class #PAFCORE_EXPORT BufferBase
	{
#{
		virtual buffer_t toBuffer() const = 0;
		virtual void fromBuffer(buffer_t buffer) = 0;
#}
	};

	class #PAFCORE_EXPORT Iterator
	{
#{
		virtual bool isEnd() const = 0;
		virtual void moveNext() = 0;
		virtual void reset() = 0;
		virtual bool equal(Iterator* other) const = 0;
#}
	};


	class(noncopyable) #PAFCORE_EXPORT Introspectable ## : public IntrospectableInterface
	{
#{
	public:
		bool isTypeOf(ClassType* classType);

		template<typename T>
		bool isTypeOf()
		{
			return isTypeOf(T::GetType());
		}

		bool isStrictTypeOf(ClassType* classType)
		{
			return getType() == classType;
		}

		template<typename T>
		bool isStrictTypeOf()
		{
			return isStrictTypeOf(T::GetType());
		}

		void* castTo(ClassType* classType);

		template<typename T>
		T* castTo()
		{
			return reinterpret_cast<T*>(castTo(T::GetType()));
		}
#}
	};

#{
	template<typename C, typename I = C::iterator>
	class IteratorImpl : public Iterator
	{
	public:
		typedef C ContainerType;
		typedef I IteratorType;
		typedef IteratorImpl<C, I> ThisType;
	public:
		IteratorImpl(C* container) :
			m_container(container),
			m_end(container->end()),
			m_iterator(m_container->begin())
		{}

		IteratorImpl(C* container, const I& iterator) :
			m_container(container),
			m_end(container->end()),
			m_iterator(iterator)
		{}

		IteratorImpl(C* container, const I& end, const I& iterator) :
			m_container(container),
			m_end(end),
			m_iterator(iterator)
		{}
	public:
		virtual bool isEnd() const
		{
			return (m_end == m_iterator);
		}
		virtual void moveNext()
		{
			if (m_end != m_iterator)
			{
				++m_iterator;
			}
		}
		virtual void reset()
		{
			//m_end = m_container->end();
			m_iterator = m_container->begin();
		}
		virtual bool equal(Iterator* other) const
		{
			return (static_cast<ThisType*>(other)->m_iterator == m_iterator);
		}
	public:
		ContainerType* getContainer()
		{
			return m_container;
		}
		IteratorType& getIterator()
		{
			return m_iterator;
		}
	protected:
		ContainerType* m_container;
		IteratorType m_end;
		IteratorType m_iterator;
	};

	template<typename T>
	class ArrayIteratorImpl : public Iterator
	{
	public:
		typedef ArrayIteratorImpl<T> ThisType;
	public:
		ArrayIteratorImpl(T* begin, size_t size) :
			m_begin(begin),
			m_size(size),
			m_index(0)
		{}
		ArrayIteratorImpl(T* begin, size_t size, size_t index) :
			m_begin(begin),
			m_size(size),
			m_index(index)
		{}
	public:
		virtual bool isEnd() const
		{
			return (m_index == m_size);
		}
		virtual void moveNext()
		{
			if (m_index != m_size)
			{
				++m_index;
			}
		}
		virtual void reset()
		{
			m_index = 0;
		}
		virtual bool equal(Iterator* other) const
		{
			ThisType* that = static_cast<ThisType*>(other);
			return (that->m_begin + that->m_index == m_begin + m_index);
		}
	public:
		T* getBegin()
		{
			return m_begin;
		}
		size_t getIndex()
		{
			return m_index;
		}
	protected:
		T* m_begin;
		size_t m_size;
		size_t m_index;
	};

	template<typename C, typename I>
	static IteratorImpl<C, I> MakeIterator(C* c, const I& i)
	{
		return IteratorImpl<C, I>(c, i);
	}
	template<typename C>
	static IteratorImpl<C, typename C::iteartor> MakeIterator(C* c)
	{
		return IteratorImpl<C>IteratorImpl(c);
	}

	template<typename T>
	static ArrayIteratorImpl<T> MakeIterator(T* begin, size_t size)
	{
		return ArrayIteratorImpl<T>(begin, size);
	}

	template<typename T>
	static ArrayIteratorImpl<T> MakeIterator(T* begin, size_t size, size_t index)
	{
		return ArrayIteratorImpl<T>(begin, size, index);
	}

#}

}

