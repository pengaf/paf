#import "Type.i"

#{
#include "Variant.h"
#include "Argument.h"
#include "Result.h"
#include "InstanceMethod.h"
#include "StaticMethod.h"
#include "NameSpace.h"

#pragma warning( push )
#pragma warning( disable : 4804 )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4146 )
#}

namespace pafcore
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
		//string_type,
		primitive_type_count,
	};	
#}

	class(primitive_type)#PAFCORE_EXPORT PrimitiveType : Type
	{
		size_t _getMemberCount_();
		Metadata* _getMember_(size_t index);
		Metadata* _findMember_(const char* name);
#{
	public:
		PrimitiveType(const char* name) : Type(name, primitive, "")
		{}
	public:
		virtual void castTo(void* dst, PrimitiveType* dstType, const void* src) = 0;
	public:
		InstanceMethod * findInstanceMethod(const char* name);
		StaticMethod* findStaticMethod(const char* name);
		Metadata* findTypeMember(const char* name);
		virtual Metadata* findMember(const char* name);
	public:
		PrimitiveTypeCategory getPrimitiveTypeCategory() const
		{
			return m_typeCategory;
		}
		//bool isString() const
		//{
		//	return (string_type == m_typeCategory);
		//}
	public:
		PrimitiveTypeCategory m_typeCategory;
		Metadata** m_members;
		size_t m_memberCount;
		InstanceMethod* m_instanceMethods;
		size_t m_instanceMethodCount;
		StaticMethod* m_staticMethods;
#}
		size_t m_staticMethodCount;
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
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<char>
	{
		enum { type_category = char_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned char>
	{
		enum { type_category = unsigned_char_type };
	};


	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<signed char>
	{
		enum { type_category = signed_char_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<wchar_t>
	{
		enum { type_category = wchar_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<short>
	{
		enum { type_category = short_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned short>
	{
		enum { type_category = unsigned_short_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<int>
	{
		enum { type_category = int_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned int>
	{
		enum { type_category = unsigned_int_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long>
	{
		enum { type_category = long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned long>
	{
		enum { type_category = unsigned_long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long long>
	{
		enum { type_category = long_long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<unsigned long long>
	{
		enum { type_category = unsigned_long_long_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<float>
	{
		enum { type_category = float_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<double>
	{
		enum { type_category = double_type };
	};

	template<>
	struct PAFCORE_EXPORT PrimitiveTypeTraits<long double>
	{
		enum { type_category = long_double_type };
	};

	//template<>
	//struct PAFCORE_EXPORT PrimitiveTypeTraits<string_t>
	//{
	//	enum { type_category = string_type };
	//};


	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl : public CPPPrimitiveType
	{
	public:
		PrimitiveTypeImpl(const char* name) : CPPPrimitiveType(name)
		{
			m_typeCategory = (PrimitiveTypeCategory)PrimitiveTypeTraits<T>::type_category;
			m_name = name;
			m_size = sizeof(T);

			static ::pafcore::Result s_staticResults[] =
			{
				::pafcore::Result(this, TypeCompound::unique_ptr),
				::pafcore::Result(this, TypeCompound::unique_array),
				::pafcore::Result(this, TypeCompound::shared_ptr),
				::pafcore::Result(this, TypeCompound::shared_array),
			};
			static ::pafcore::Argument s_staticArguments[] =
			{
				::pafcore::Argument("arg", this, ::pafcore::Argument::by_value, false),
				::pafcore::Argument("count", RuntimeTypeOf<unsigned int>::RuntimeType::GetSingleton(), ::pafcore::Argument::by_value, false),
			};
			static ::pafcore::Overload s_staticOverloads[] =
			{
				::pafcore::Overload(&s_staticResults[0], &s_staticArguments[0], 0, true, false),
				::pafcore::Overload(&s_staticResults[1], &s_staticArguments[0], 1, true, false),
				::pafcore::Overload(&s_staticResults[2], &s_staticArguments[1], 1, true, false),
			};
			static ::pafcore::StaticMethod s_staticMethods[] =
			{
				::pafcore::StaticMethod("New", 0, Primitive_New, &s_staticResults[0], &s_staticArguments[0], 0),
				::pafcore::StaticMethod("NewArray", 0, Primitive_NewArray, &s_staticOverloads[2], 1),
				::pafcore::StaticMethod("NewShared", 0, Primitive_NewShared, &s_staticOverloads[0], 2),
				::pafcore::StaticMethod("NewSharedArray", 0, Primitive_NewSharedArray, &s_staticOverloads[2], 1),
			};
			m_staticMethods = s_staticMethods;
			m_staticMethodCount = paf_array_size_of(s_staticMethods);
			static Metadata* s_members[] =
			{
				&s_staticMethods[0],
				&s_staticMethods[1],
				&s_staticMethods[2],
				&s_staticMethods[3],
			};
			m_members = s_members;
			m_memberCount = paf_array_size_of(s_members);
			NameSpace::GetGlobalNameSpace()->registerMember(this);
		}
		static ErrorCode Primitive_New(Variant* result, Variant** args, int_t numArgs)
		{
			if(1 < numArgs)
			{
				return e_invalid_arg_num;
			}
			T a0 = 0;
			if(1 == numArgs)
			{
				if(!args[0]->castToPrimitive(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &a0))
				{
					return e_invalid_arg_type_1;
				}
			}
			result->assignPrimitiveForNew(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), &a0);
			return s_ok;
		}
		static ErrorCode Primitive_NewArray(Variant* result, Variant** args, int_t numArgs)
		{
			if(1 == numArgs)
			{
				unsigned int count;
				if(!args[0]->castToPrimitive(RuntimeTypeOf<unsigned int>::RuntimeType::GetSingleton(), &count))
				{
					return e_invalid_arg_type_1;
				}
				T* p = paf_new_array<T>(count);
				result->assignArray(RuntimeTypeOf<T>::RuntimeType::GetSingleton(), p, count, false, Variant::by_new_array);
				return s_ok;
			}
			return e_invalid_arg_num;
		}
		virtual void destroyArray(void* address)
		{
			paf_delete_array((T*)address);
		}
		virtual void castTo(void* dst, PrimitiveType* dstType, const void* src)
		{
			PAF_ASSERT(dst && dstType && src);
			switch (dstType->m_typeCategory)
			{
			case bool_type:
				*reinterpret_cast<bool_t*>(dst) = *reinterpret_cast<const T*>(src) != 0;
				break;
			case char_type:
				*reinterpret_cast<char_t*>(dst) = static_cast<char_t>(*reinterpret_cast<const T*>(src));
				break;
			case signed_char_type:
				*reinterpret_cast<schar_t*>(dst) = static_cast<char_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_char_type:
				*reinterpret_cast<uchar_t*>(dst) = static_cast<uchar_t>(*reinterpret_cast<const T*>(src));
				break;
			case wchar_type:
				*reinterpret_cast<wchar_t*>(dst) = static_cast<wchar_t>(*reinterpret_cast<const T*>(src));
				break;
			case short_type:
				*reinterpret_cast<short_t*>(dst) = static_cast<short_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_short_type:
				*reinterpret_cast<ushort_t*>(dst) = static_cast<ushort_t>(*reinterpret_cast<const T*>(src));
				break;
			case int_type:
				*reinterpret_cast<int_t*>(dst) = static_cast<int_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_int_type:
				*reinterpret_cast<uint_t*>(dst) = static_cast<uint_t>(*reinterpret_cast<const T*>(src));
				break;
			case long_type:
				*reinterpret_cast<long_t*>(dst) = static_cast<long_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_long_type:
				*reinterpret_cast<ulong_t*>(dst) = static_cast<ulong_t>(*reinterpret_cast<const T*>(src));
				break;
			case long_long_type:
				*reinterpret_cast<longlong_t*>(dst) = static_cast<longlong_t>(*reinterpret_cast<const T*>(src));
				break;
			case unsigned_long_long_type:
				*reinterpret_cast<ulonglong_t*>(dst) = static_cast<ulonglong_t>(*reinterpret_cast<const T*>(src));
				break;
			case float_type:
				*reinterpret_cast<float_t*>(dst) = static_cast<float_t>(*reinterpret_cast<const T*>(src));
				break;
			case double_type:
				*reinterpret_cast<double_t*>(dst) = static_cast<double_t>(*reinterpret_cast<const T*>(src));
				break;
			case long_double_type:
				*reinterpret_cast<longdouble_t*>(dst) = static_cast<longdouble_t>(*reinterpret_cast<const T*>(src));
				break;
			default:
				PAF_ASSERT(false);
			}
		}
		virtual void op_plus(void* dst, const void* arg)
		{
			*reinterpret_cast<T*>(dst) = +*reinterpret_cast<const T*>(arg);
		}
		virtual bool op_not(const void* arg)
		{
			return !*reinterpret_cast<const T*>(arg);
		}
		virtual void op_add(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) + *reinterpret_cast<const T*>(arg2);
		}
		virtual void op_subtract(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) - *reinterpret_cast<const T*>(arg2);
		}
		virtual void op_multiply(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) * *reinterpret_cast<const T*>(arg2);
		}
		virtual void op_divide(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) / *reinterpret_cast<const T*>(arg2);
		}
		virtual bool op_less(const void* arg1, const void* arg2)
		{
			return *reinterpret_cast<const T*>(arg1) < *reinterpret_cast<const T*>(arg2);
		}
		virtual bool op_lessEqual(const void* arg1, const void* arg2)
		{
			return *reinterpret_cast<const T*>(arg1) <= *reinterpret_cast<const T*>(arg2);
		}
		virtual bool op_equal(const void* arg1, const void* arg2)
		{
			return *reinterpret_cast<const T*>(arg1) == *reinterpret_cast<const T*>(arg2);
		}
		virtual bool op_notEqual(const void* arg1, const void* arg2)
		{
			return *reinterpret_cast<const T*>(arg1) != *reinterpret_cast<const T*>(arg2);
		}
		virtual bool op_greaterEqual(const void* arg1, const void* arg2)
		{
			return *reinterpret_cast<const T*>(arg1) >= *reinterpret_cast<const T*>(arg2);
		}
		virtual bool op_greater(const void* arg1, const void* arg2)
		{
			return *reinterpret_cast<const T*>(arg1) > *reinterpret_cast<const T*>(arg2);
		}
	};
	
	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl_Integer : public PrimitiveTypeImpl<T>
	{
	public:
		PrimitiveTypeImpl_Integer(const char* name) : PrimitiveTypeImpl(name)
		{}
		virtual void op_increment(void* dst, void* arg)
		{
			*reinterpret_cast<T*>(dst) = ++(*reinterpret_cast<T*>(arg));
		}
		virtual void op_postIncrement(void* dst, void* arg)
		{
			*reinterpret_cast<T*>(dst) = (*reinterpret_cast<T*>(arg))++;
		}
		virtual void op_bitwiseNot(void* dst, const void* arg)
		{
			*reinterpret_cast<T*>(dst) = ~*reinterpret_cast<const T*>(arg);
		}
		virtual void op_mod(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) % *reinterpret_cast<const T*>(arg2);
		}
		virtual void op_bitwiseAnd(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) & *reinterpret_cast<const T*>(arg2);
		}
		virtual void op_bitwiseOr(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) | *reinterpret_cast<const T*>(arg2);
		}
		virtual void op_bitwiseXor(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) ^ *reinterpret_cast<const T*>(arg2);
		}
		virtual void op_leftShift(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) << *reinterpret_cast<const char*>(arg2);
		}
		virtual void op_rightShift(void* dst, const void* arg1, const void* arg2)
		{
			*reinterpret_cast<T*>(dst) = *reinterpret_cast<const T*>(arg1) >> *reinterpret_cast<const char*>(arg2);
		}
	};

	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl_Bool : public PrimitiveTypeImpl_Integer<T>
	{
	public:
		typedef PrimitiveTypeImpl_Bool<T> ThisType;
	public:
		PrimitiveTypeImpl_Bool(const char* name) : PrimitiveTypeImpl_Integer(name)
		{}
		static ThisType s_instance;
		static ThisType* GetSingleton()
		{
			return &s_instance;
		}
		virtual void op_negate(void* dst, const void* arg)
		{
			*reinterpret_cast<T*>(dst) = -*reinterpret_cast<const T*>(arg);
		}
		virtual void op_decrement(void* dst, void* arg)
		{}
		virtual void op_postDecrement(void* dst, void* arg)
		{}
	};

	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl_ForbidNegate : public PrimitiveTypeImpl_Integer<T>
	{
	public:
		typedef PrimitiveTypeImpl_ForbidNegate<T> ThisType;
	public:
		PrimitiveTypeImpl_ForbidNegate(const char* name) : PrimitiveTypeImpl_Integer(name)
		{}
		static ThisType s_instance;
		static ThisType* GetSingleton()
		{
			return &s_instance;
		}
		virtual void op_negate(void* dst, const void* arg)
		{}
		virtual void op_decrement(void* dst, void* arg)
		{
			*reinterpret_cast<T*>(dst) = --*reinterpret_cast<T*>(arg);
		}
		virtual void op_postDecrement(void* dst, void* arg)
		{
			*reinterpret_cast<T*>(dst) = (*reinterpret_cast<T*>(arg))--;
		}
	};

	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl_AllowNegate : public PrimitiveTypeImpl_Integer<T>
	{
	public:
		typedef PrimitiveTypeImpl_AllowNegate<T> ThisType;
	public:
		PrimitiveTypeImpl_AllowNegate(const char* name) : PrimitiveTypeImpl_Integer(name)
		{}
		static ThisType s_instance;
		static ThisType* GetSingleton()
		{
			return &s_instance;
		}
		virtual void op_negate(void* dst, const void* arg)
		{
			*reinterpret_cast<T*>(dst) = -*reinterpret_cast<const T*>(arg);
		}
		virtual void op_decrement(void* dst, void* arg)
		{
			*reinterpret_cast<T*>(dst) = --*reinterpret_cast<T*>(arg);
		}
		virtual void op_postDecrement(void* dst, void* arg)
		{
			*reinterpret_cast<T*>(dst) = (*reinterpret_cast<T*>(arg))--;
		}
	};

	template<typename T>
	class PAFCORE_EXPORT PrimitiveTypeImpl_Real : public PrimitiveTypeImpl<T>
	{
	public:
		typedef PrimitiveTypeImpl_Real<T> ThisType;
	public:
		PrimitiveTypeImpl_Real(const char* name) : PrimitiveTypeImpl(name)
		{}
		static ThisType s_instance;
		static ThisType* GetSingleton()
		{
			return &s_instance;
		}
		virtual void op_negate(void* dst, const void* arg)
		{
			*reinterpret_cast<T*>(dst) = -*reinterpret_cast<const T*>(arg);
		}
		virtual void op_increment(void* dst, void* arg)
		{}
		virtual void op_postIncrement(void* dst, void* arg)
		{}
		virtual void op_decrement(void* dst, void* arg)
		{}
		virtual void op_postDecrement(void* dst, void* arg)
		{}
		virtual void op_bitwiseNot(void* dst, const void* arg)
		{}
		void op_mod(void* dst, const void* arg1, const void* arg2)
		{}
		virtual void op_bitwiseAnd(void* dst, const void* arg1, const void* arg2)
		{}
		virtual void op_bitwiseOr(void* dst, const void* arg1, const void* arg2)
		{}
		virtual void op_bitwiseXor(void* dst, const void* arg1, const void* arg2)
		{}
		virtual void op_leftShift(void* dst, const void* arg1, const void* arg2)
		{}
		virtual void op_rightShift(void* dst, const void* arg1, const void* arg2)
		{}
	};

	typedef PrimitiveTypeImpl_Bool<bool>						BoolType;
	typedef PrimitiveTypeImpl_AllowNegate<char>					CharType;
	typedef PrimitiveTypeImpl_AllowNegate<signed char>			SignedCharType;
	typedef PrimitiveTypeImpl_AllowNegate<unsigned char>		UnsignedCharType;
	typedef PrimitiveTypeImpl_AllowNegate<wchar_t>				WcharType;
	typedef PrimitiveTypeImpl_AllowNegate<short>				ShortType;
	typedef PrimitiveTypeImpl_AllowNegate<unsigned short>		UnsignedShortType;
	typedef PrimitiveTypeImpl_AllowNegate<long>					LongType;
	typedef PrimitiveTypeImpl_ForbidNegate<unsigned long>		UnsignedLongType;
	typedef PrimitiveTypeImpl_AllowNegate<long long>			LongLongType;
	typedef PrimitiveTypeImpl_ForbidNegate<unsigned long long>	UnsignedLongLongType;
	typedef PrimitiveTypeImpl_AllowNegate<int>					IntType;
	typedef PrimitiveTypeImpl_ForbidNegate<unsigned int>		UnsignedIntType;
	typedef PrimitiveTypeImpl_Real<float>						FloatType;
	typedef PrimitiveTypeImpl_Real<double>						DoubleType;
	typedef PrimitiveTypeImpl_Real<long double>					LongDoubleType;



	//class PAFCORE_EXPORT StringType : public PrimitiveType
	//{
	//public:
	//	StringType(const char* name) : PrimitiveType(name)
	//	{
	//		m_typeCategory = (PrimitiveTypeCategory)PrimitiveTypeTraits<::string_t>::type_category;
	//		m_name = name;
	//		m_size = sizeof(::string_t);

	//		static ::pafcore::Result s_staticResults[] =
	//		{
	//			::pafcore::Result(this, false, ::pafcore::Result::by_new),
	//			::pafcore::Result(this, false, ::pafcore::Result::by_new),
	//		};
	//		static ::pafcore::Argument s_staticArguments[] =
	//		{
	//			::pafcore::Argument("str", CharType::GetSingleton(), ::pafcore::Argument::by_ptr, true),
	//		};
	//		static ::pafcore::Overload s_staticOverloads[] =
	//		{
	//			::pafcore::Overload(&s_staticResults[0], 0, 0, true, false),
	//			::pafcore::Overload(&s_staticResults[1], &s_staticArguments[0], 1, true, false),
	//		};
	//		static ::pafcore::StaticMethod s_staticMethods[] =
	//		{
	//			::pafcore::StaticMethod("New", 0, string_t_New, &s_staticOverloads[0], 2),
	//		};
	//		m_staticMethods = s_staticMethods;
	//		m_staticMethodCount = paf_array_size_of(s_staticMethods);
	//		static Metadata* s_members[] =
	//		{
	//			&s_staticMethods[0],
	//		};
	//		m_members = s_members;
	//		m_memberCount = paf_array_size_of(s_members);
	//		::pafcore::NameSpace::GetGlobalNameSpace()->registerMember(this);
	//	}
	//	static ErrorCode string_t_New(::pafcore::Variant* result, ::pafcore::Variant** args, int_t numArgs)
	//	{
	//		if (0 == numArgs)
	//		{
	//			result->assignNullPrimitive(GetSingleton());
	//			return ::pafcore::s_ok;
	//		}
	//		if (1 <= numArgs)
	//		{
	//			if (args[0]->isTemporary())
	//			{
	//				return ::pafcore::e_invalid_arg_type_1;
	//			}
	//			const char* a0;
	//			if (!args[0]->castToPrimitivePtr(CharType::GetSingleton(), (void**)&a0))
	//			{
	//				return ::pafcore::e_invalid_arg_type_1;
	//			}
	//			::string_t res(a0);
	//			result->assignPrimitive(GetSingleton(), &res);
	//			return ::pafcore::s_ok;
	//		}
	//		return ::pafcore::e_invalid_arg_num;
	//	}

	//	virtual bool castTo(void* dst, PrimitiveType* dstType, const void* src)
	//	{
	//		if (!dstType->isPrimitive())
	//		{
	//			return false;
	//		}
	//		if (string_type != static_cast<PrimitiveType*>(dstType)->m_typeCategory)
	//		{
	//			return false;
	//		}
	//		*reinterpret_cast<::string_t*>(dst) = (*reinterpret_cast<const ::string_t*>(src));
	//		return true;
	//	}

	//public:
	//	static StringType s_instance;
	//	static StringType* GetSingleton()
	//	{
	//		return &s_instance;
	//	}
	//};


#}
}

#{
template<>
struct RuntimeTypeOf<bool>
{
	typedef ::pafcore::BoolType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<char>
{
	typedef ::pafcore::CharType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<signed char>
{
	typedef ::pafcore::SignedCharType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<unsigned char>
{
	typedef ::pafcore::UnsignedCharType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<wchar_t>
{
	typedef ::pafcore::WcharType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<short>
{
	typedef ::pafcore::ShortType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<unsigned short>
{
	typedef ::pafcore::UnsignedShortType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<long>
{
	typedef ::pafcore::LongType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<unsigned long>
{
	typedef ::pafcore::UnsignedLongType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<long long>
{
	typedef ::pafcore::LongLongType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<unsigned long long>
{
	typedef ::pafcore::UnsignedLongLongType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<int>
{
	typedef ::pafcore::IntType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<unsigned int>
{
	typedef ::pafcore::UnsignedIntType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<float>
{
	typedef ::pafcore::FloatType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<double>
{
	typedef ::pafcore::DoubleType RuntimeType;
	enum {type_category = ::pafcore::primitive};
};

template<>
struct RuntimeTypeOf<long double>
{
	typedef ::pafcore::LongDoubleType RuntimeType;
	enum { type_category = ::pafcore::primitive };
};

//template<>
//struct RuntimeTypeOf<string_t>
//{
//	typedef ::pafcore::StringType RuntimeType;
//	enum { type_category = ::pafcore::primitive };
//};

template<typename T>
struct RuntimeTypeOf<T*>
{
	typedef RuntimeTypeOf<size_t>::RuntimeType RuntimeType;
	enum { type_category = ::pafcore::primitive };
};

#pragma warning( pop ) 
#}