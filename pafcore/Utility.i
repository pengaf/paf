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

#define BEGIN_PAFCORE namespace pafcore {
#define END_PAFCORE }

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

template<typename T, bool b = std::is_copy_constructible_v<T>>
struct CopyConstructorCaller
{
	static bool Call(void* dst, const void* src)
	{
		new(dst)T(*reinterpret_cast<const T*>(src));
		return true;
	}
};

template<typename T>
struct CopyConstructorCaller<T, false>
{
	static bool Call(void* dst, const void* src)
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

namespace pafcore
{
#{
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
#}

	class string_t
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


}

