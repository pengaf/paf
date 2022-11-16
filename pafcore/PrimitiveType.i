#import "Type.i"

#{
#include "Variant.h"
#include "Method.h"
#include "NameSpace.h"

#}

namespace paf
{
#{
	enum PrimitiveTypeCategory
	{
		bool_type,
		char_type,
		signed_char_type,
		unsigned_char_type,
		wchar_type,
		short_type,
		unsigned_short_type,
		int_type,
		unsigned_int_type,
		long_type,
		unsigned_long_type,
		long_long_type,
		unsigned_long_long_type,
		float_type,
		double_type,
		long_double_type,
		primitive_type_count,
	};	
#}

	class(primitive_type)#PAFCORE_EXPORT PrimitiveType : Type
	{
		size_t _getMemberCount_();
		Metadata* _getMember_(size_t index);
		Metadata* _findMember_(string_t name);
#{
	public:
		PrimitiveType(const char* name) : Type(name, MetaCategory::primitive, "")
		{}
	//public:
	//	virtual void castTo(void* dst, PrimitiveType* dstType, const void* src) = 0;
	public:
		//StaticMethod* findStaticMethod(const char* name);
		//Metadata* findTypeMember(const char* name);
		//virtual Metadata* findMember(const char* name)
		//{
		//	return nullptr;
		//}
	public:
		PrimitiveTypeCategory getPrimitiveTypeCategory() const
		{
			return m_typeCategory;
		}
		//size_t getStaticMethodCount() const
		//{
		//	return m_staticMethodCount;
		//}
		//StaticMethod* getStaticMethod(size_t index) const
		//{
		//	return index < m_staticMethodCount ? &m_staticMethods[index] : nullptr;
		//}
	protected:
		PrimitiveTypeCategory m_typeCategory;
		//Metadata** m_members;
		//size_t m_memberCount;
		//StaticMethod* m_staticMethods;
		//size_t m_staticMethodCount;
#}
	};
#{

	class PAFCORE_EXPORT CPPPrimitiveType : public PrimitiveType
	{
	public:
		CPPPrimitiveType(const char* name) : PrimitiveType(name)
		{}
	};

	template<typename T>
	struct PAFCORE_EXPORT PrimitiveTypeTraits
	{
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<bool>
	{
		enum { type_category = bool_type };
		//inline static const char* s_name = "bool";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<char>
	{
		enum { type_category = char_type };
		//inline static const char* s_name = "char";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned char>
	{
		enum { type_category = unsigned_char_type };
		//inline static const char* s_name = "unsigned char";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<signed char>
	{
		enum { type_category = signed_char_type };
		//inline static const char* s_name = "signed char";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<wchar_t>
	{
		enum { type_category = wchar_type };
		//inline static const char* s_name = "wchar_t";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<short>
	{
		enum { type_category = short_type };
		//inline static const char* s_name = "short";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned short>
	{
		enum { type_category = unsigned_short_type };
		//inline static const char* s_name = "unsigned short";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<int>
	{
		enum { type_category = int_type };
		//inline static const char* s_name = "int";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned int>
	{
		enum { type_category = unsigned_int_type };
		//inline static const char* s_name = "unsigned int";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long>
	{
		enum { type_category = long_type };
		//inline static const char* s_name = "long";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned long>
	{
		enum { type_category = unsigned_long_type };
		//inline static const char* s_name = "unsigned long";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long long>
	{
		enum { type_category = long_long_type };
		//inline static const char* s_name = "long long";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned long long>
	{
		enum { type_category = unsigned_long_long_type };
		//inline static const char* s_name = "unsigned long long";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<float>
	{
		enum { type_category = float_type };
		//inline static const char* s_name = "float";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<double>
	{
		enum { type_category = double_type };
		//inline static const char* s_name = "double";
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long double>
	{
		enum { type_category = long_double_type };
		//inline static const char* s_name = "long double";
	};

	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl : public CPPPrimitiveType
	{
	public:
		PrimitiveTypeImpl(const char* name) : CPPPrimitiveType(name)
		{
			m_typeCategory = (PrimitiveTypeCategory)PrimitiveTypeTraits<T>::type_category;
			m_name = name;
			m_size = sizeof(T);
			NameSpace::GetGlobalNameSpace()->registerMember(this);
		}
		virtual ErrorCode placementNew(void* address, ::paf::Variant** args, uint32_t numArgs) override
		{
			if (0 == numArgs)
			{
				return ErrorCode::s_ok;
			}
			T a0;
			if (!args[0]->castToValue(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &a0))
			{
				return ErrorCode::e_invalid_arg_type_1;
			}
			if (1 == numArgs)
			{
				new(address)T(a0);
				return ErrorCode::s_ok;
			}
			return ErrorCode::e_too_many_arguments;
		}
		virtual bool placementNewArray(void* address, size_t count) override
		{
			new(address)T[count];
			return true;
		}
		virtual bool destruct(void* self) override
		{
			return true;
		}
		//virtual bool copyConstruct(void* dst, const void* src) override
		//{
		//	*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(src);
		//	return true;
		//}
		virtual bool copyAssign(void* self, const void* src) override
		{
			*reinterpret_cast<T*>(self) = *reinterpret_cast<const T*>(src);
			return true;
		}
		virtual bool assign(void* self, Type* srcType, const void* src) override
		{
			PAF_ASSERT(self && srcType && src);
			if (!srcType->isPrimitive())
			{
				return false;
			}
			switch (static_cast<PrimitiveType*>(srcType)->getPrimitiveTypeCategory())
			{
			case bool_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const bool_t*>(src));
				break;
			case char_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const char_t*>(src));
				break;
			case signed_char_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const schar_t*>(src));
				break;
			case unsigned_char_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const uchar_t*>(src));
				break;
			case wchar_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const wchar_t*>(src));
				break;
			case short_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const short_t*>(src));
				break;
			case unsigned_short_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const ushort_t*>(src));
				break;
			case int_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const int_t*>(src));
				break;
			case unsigned_int_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const uint_t*>(src));
				break;
			case long_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const long_t*>(src));
				break;
			case unsigned_long_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const ulong_t*>(src));
				break;
			case long_long_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const longlong_t*>(src));
				break;
			case unsigned_long_long_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const ulonglong_t*>(src));
				break;
			case float_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const float_t*>(src));
				break;
			case double_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const double_t*>(src));
				break;
			case long_double_type:
				*reinterpret_cast<T*>(self) = static_cast<T>(*reinterpret_cast<const longdouble_t*>(src));
				break;
			default:
				PAF_ASSERT(false);
			}
			return true;
		}
		virtual bool cast(Type* dstType, void* dst, const void* self) override
		{
			PAF_ASSERT(dst && dstType && self);
			if (!dstType->isPrimitive())
			{
				return false;
			}
			switch (static_cast<PrimitiveType*>(dstType)->getPrimitiveTypeCategory())
			{
			case bool_type:
				*reinterpret_cast<bool_t*>(dst) = static_cast<bool_t>(*reinterpret_cast<const T*>(self));
				break;
			case char_type:
				*reinterpret_cast<char_t*>(dst) = static_cast<char_t>(*reinterpret_cast<const T*>(self));
				break;
			case signed_char_type:
				*reinterpret_cast<schar_t*>(dst) = static_cast<schar_t>(*reinterpret_cast<const T*>(self));
				break;
			case unsigned_char_type:
				*reinterpret_cast<uchar_t*>(dst) = static_cast<uchar_t>(*reinterpret_cast<const T*>(self));
				break;
			case wchar_type:
				*reinterpret_cast<wchar_t*>(dst) = static_cast<wchar_t>(*reinterpret_cast<const T*>(self));
				break;
			case short_type:
				*reinterpret_cast<short_t*>(dst) = static_cast<short_t>(*reinterpret_cast<const T*>(self));
				break;
			case unsigned_short_type:
				*reinterpret_cast<ushort_t*>(dst) = static_cast<ushort_t>(*reinterpret_cast<const T*>(self));
				break;
			case int_type:
				*reinterpret_cast<int_t*>(dst) = static_cast<int_t>(*reinterpret_cast<const T*>(self));
				break;
			case unsigned_int_type:
				*reinterpret_cast<uint_t*>(dst) = static_cast<uint_t>(*reinterpret_cast<const T*>(self));
				break;
			case long_type:
				*reinterpret_cast<long_t*>(dst) = static_cast<long_t>(*reinterpret_cast<const T*>(self));
				break;
			case unsigned_long_type:
				*reinterpret_cast<ulong_t*>(dst) = static_cast<ulong_t>(*reinterpret_cast<const T*>(self));
				break;
			case long_long_type:
				*reinterpret_cast<longlong_t*>(dst) = static_cast<longlong_t>(*reinterpret_cast<const T*>(self));
				break;
			case unsigned_long_long_type:
				*reinterpret_cast<ulonglong_t*>(dst) = static_cast<ulonglong_t>(*reinterpret_cast<const T*>(self));
				break;
			case float_type:
				*reinterpret_cast<float_t*>(dst) = static_cast<float_t>(*reinterpret_cast<const T*>(self));
				break;
			case double_type:
				*reinterpret_cast<double_t*>(dst) = static_cast<double_t>(*reinterpret_cast<const T*>(self));
				break;
			case long_double_type:
				*reinterpret_cast<longdouble_t*>(dst) = static_cast<longdouble_t>(*reinterpret_cast<const T*>(self));
				break;
			default:
				PAF_ASSERT(false);
			}
			return true;
		}
	public:
		static PrimitiveTypeImpl s_instance;
		static PrimitiveTypeImpl* GetSingleton()
		{
			return &s_instance;
		}
		//static PrimitiveTypeImpl* GetSingleton()
		//{
		//	static PrimitiveTypeImpl* s_instance = 0;
		//	static char s_buffer[sizeof(PrimitiveTypeImpl)];
		//	if (0 == s_instance)
		//	{
		//		s_instance = (PrimitiveTypeImpl*)s_buffer;
		//		new (s_buffer)PrimitiveTypeImpl(PrimitiveTypeTraits<T>::s_name);
		//	}
		//	return s_instance;
		//}
	};
	

	typedef PrimitiveTypeImpl<bool>					BoolType;
	typedef PrimitiveTypeImpl<char>					CharType;
	typedef PrimitiveTypeImpl<signed char>			SignedCharType;
	typedef PrimitiveTypeImpl<unsigned char>		UnsignedCharType;
	typedef PrimitiveTypeImpl<wchar_t>				WcharType;
	typedef PrimitiveTypeImpl<short>				ShortType;
	typedef PrimitiveTypeImpl<unsigned short>		UnsignedShortType;
	typedef PrimitiveTypeImpl<long>					LongType;
	typedef PrimitiveTypeImpl<unsigned long>		UnsignedLongType;
	typedef PrimitiveTypeImpl<long long>			LongLongType;
	typedef PrimitiveTypeImpl<unsigned long long>	UnsignedLongLongType;
	typedef PrimitiveTypeImpl<int>					IntType;
	typedef PrimitiveTypeImpl<unsigned int>			UnsignedIntType;
	typedef PrimitiveTypeImpl<float>				FloatType;
	typedef PrimitiveTypeImpl<double>				DoubleType;
	typedef PrimitiveTypeImpl<long double>			LongDoubleType;

#}
}

#{
template<>
struct RuntimeTypeOf<bool>
{
	typedef ::paf::BoolType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<char>
{
	typedef ::paf::CharType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<signed char>
{
	typedef ::paf::SignedCharType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<unsigned char>
{
	typedef ::paf::UnsignedCharType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<wchar_t>
{
	typedef ::paf::WcharType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<short>
{
	typedef ::paf::ShortType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<unsigned short>
{
	typedef ::paf::UnsignedShortType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<long>
{
	typedef ::paf::LongType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<unsigned long>
{
	typedef ::paf::UnsignedLongType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<long long>
{
	typedef ::paf::LongLongType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<unsigned long long>
{
	typedef ::paf::UnsignedLongLongType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<int>
{
	typedef ::paf::IntType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<unsigned int>
{
	typedef ::paf::UnsignedIntType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<float>
{
	typedef ::paf::FloatType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<double>
{
	typedef ::paf::DoubleType RuntimeType;
	enum {type_category = ::paf::MetaCategory::primitive};
};

template<>
struct RuntimeTypeOf<long double>
{
	typedef ::paf::LongDoubleType RuntimeType;
	enum { type_category = ::paf::MetaCategory::primitive };
};

#}