#{
#include "Utility.h"
#include <string.h>

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

#ifdef _WIN64
typedef unsigned __int64		size_t;
typedef __int64					ptrdiff_t;
#else
typedef unsigned int			size_t;
typedef int						ptrdiff_t;
#endif

//只记录指针，没有内存所有权，外界使用时注意生命期
class string_t
{
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
	bool operator == (const char* str) const
	{
		return 0 == strcmp(m_str, str);
	}
	bool operator != (const char* str) const
	{
		return 0 != strcmp(m_str, str);
	}
	size_t length() const
	{
		return strlen(m_str);
	}
protected:
	const char* m_str;
};

template<typename T>
struct RuntimeTypeOf
{};

#}

primitive bool_t;
primitive char_t;
primitive schar_t;
primitive uchar_t;
primitive short_t;
primitive ushort_t;
primitive long_t;
primitive ulong_t;
primitive longlong_t;
primitive ulonglong_t;
primitive int_t;
primitive uint_t;
primitive float_t;
primitive double_t;
primitive longdouble_t;
primitive byte_t;
primitive int8_t;
primitive uint8_t;
primitive int16_t;
primitive uint16_t;
primitive int32_t;
primitive uint32_t;
primitive int64_t;
primitive uint64_t;
primitive size_t;
primitive ptrdiff_t;

struct Nil
{
};

namespace pafcore
{
	//只记录指针，没有内存所有权，外界使用时注意生命期
	//在序列化时保持格式(xml cdata)
	class(value_object) #PAFCORE_EXPORT text_t
	{
		nocode text_t();
#{
	public:
		text_t() : m_str("")
		{}
		text_t(const char* str) : m_str(str ? str : "")
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
		bool operator == (const char* str) const
		{
			return 0 == strcmp(m_str, str);
		}
		bool operator != (const char* str) const
		{
			return 0 != strcmp(m_str, str);
		}
		size_t length() const
		{
			return strlen(m_str);
		}
	protected:
		const char* m_str;
#}
	};

	//只记录指针，没有内存所有权，外界使用时注意生命期
	class(value_object) #PAFCORE_EXPORT buffer_t
	{
		nocode buffer_t();
#{
	public:
		buffer_t() : buffer_t(0, 0)
		{}
		buffer_t(void* pointer, size_t size) : m_pointer(pointer), m_size(size)
		{}
	public:
		void* getData() const
		{
			return m_pointer;
		}
		size_t getLength() const
		{
			return m_size;
		}
		void assign(void* pointer, size_t size)
		{
			m_pointer = pointer;
			m_size = size;
		}
	protected:
		void* m_pointer;
		size_t m_size;
#}
	};

	//用于界面执行方法，其中成员用于不能立即完成的操作,调用者收到属性变更通知时调用 get 方法时返回
	//立即完成的操作,get方法保持返回 Status::ready 即可
	//属性的 set 方法忽略其中成员
	struct #PAFCORE_EXPORT MethodAsProperty
	{
		//不要暴露构造函数，避免在反序列化时调用 set_
#{
		enum Status : uint8_t
		{
			ready,
			running,
			running_allow_cancel,
		};
		Status status;
		float progress;
		MethodAsProperty() : status(ready), progress(0)
		{}
#}
	};

}