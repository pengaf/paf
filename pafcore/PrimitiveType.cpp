#include "PrimitiveType.h"
#include "PrimitiveType.mh"
#include "PrimitiveType.ic"
#include "PrimitiveType.mc"
#include "TypeAlias.h"
#include <algorithm>

BEGIN_PAF

size_t PrimitiveType::_getMemberCount_()
{
	return 0;
}

Metadata* PrimitiveType::_getMember_(size_t index)
{
	return nullptr;
}

Metadata* PrimitiveType::_findMember_(string_t name)
{
	return nullptr;
}

//size_t PrimitiveType::_getMemberCount_()
//{
//	return m_memberCount;
//}
//
//Metadata* PrimitiveType::_getMember_(size_t index)
//{
//	if (index < m_memberCount)
//	{
//		return m_members[index];
//	}
//	return nullptr;
//}
//
//Metadata* PrimitiveType::_findMember_(string_t name)
//{
//	Metadata dummy(name);
//	Metadata** it = std::lower_bound(m_members, m_members + m_memberCount, &dummy, CompareMetaDataPtrByName());
//	if (m_members + m_memberCount != it && strcmp(name, (*it)->m_name) == 0)
//	{
//		return *it;
//	}
//	return nullptr;
//}
//
//Metadata* PrimitiveType::findMember(const char* name)
//{
//	return _findMember_(name);
//}
//
//InstanceMethod* PrimitiveType::findInstanceMethod(const char* name)
//{
//	Metadata dummy(name);
//	InstanceMethod* res = std::lower_bound(m_instanceMethods, m_instanceMethods + m_instanceMethodCount, dummy);
//	if (m_instanceMethods + m_instanceMethodCount != res && strcmp(name, res->m_name) == 0)
//	{
//		return res;
//	}
//	return 0;
//}

//StaticMethod* PrimitiveType::findStaticMethod(const char* name)
//{
//	Metadata dummy(name);
//	StaticMethod* res = std::lower_bound(m_staticMethods, m_staticMethods + m_staticMethodCount, dummy);
//	if (m_staticMethods + m_staticMethodCount != res && strcmp(name, res->m_name) == 0)
//	{
//		return res;
//	}
//	return 0;
//}
//
//Metadata* PrimitiveType::findTypeMember(const char* name)
//{
//	return findStaticMethod(name);
//}

//template<bool short_less_int>
//struct TypePromoter
//{
//	static PrimitiveTypeCategory GetPromotedTypeCategory(PrimitiveTypeCategory typeCategory)
//	{
//		if (typeCategory <= unsigned_short_type)
//		{
//			return int_type;
//		}
//		return typeCategory;
//	}
//};
//
//template<>
//struct TypePromoter<false>
//{
//	static PrimitiveTypeCategory GetPromotedTypeCategory(PrimitiveTypeCategory typeCategory)
//	{
//		if (typeCategory < unsigned_short_type)
//		{
//			return int_type;
//		}
//		else if (typeCategory == unsigned_short_type)
//		{
//			return unsigned_int_type;
//		}
//		return typeCategory;
//	}
//};
//
//inline PrimitiveTypeCategory GetPromotedTypeCategory(PrimitiveTypeCategory typeCategory)
//{
//	return TypePromoter<sizeof(unsigned short) < sizeof(int)>::GetPromotedTypeCategory(typeCategory);
//}
//
//inline PrimitiveTypeCategory GetPromotedTypeCategory(PrimitiveTypeCategory typeCategory1, PrimitiveTypeCategory typeCategory2)
//{
//	PrimitiveTypeCategory small, large; 
//	if(typeCategory1 < typeCategory2)
//	{
//		small = typeCategory1;
//		large = typeCategory2;
//	}
//	else
//	{
//		small = typeCategory2;
//		large = typeCategory1;
//	}
//	if(unsigned_int_type == small && long_type == large)
//	{
//		return unsigned_long_type;
//	}
//	return GetPromotedTypeCategory(large);
//}

CPPPrimitiveType* GetPrimitiveTypeFromTypeCategory(PrimitiveTypeCategory typeCategory)
{
	CPPPrimitiveType* res = 0;
	switch(typeCategory)
	{
	case bool_type:
		res = BoolType::GetSingleton();
		break;
	case char_type:
		res = CharType::GetSingleton();
		break;
	case signed_char_type:
		res = SignedCharType::GetSingleton();
		break;
	case unsigned_char_type:
		res = UnsignedCharType::GetSingleton();
		break;
	case wchar_type:
		res = WcharType::GetSingleton();
		break;
	case short_type:
		res = ShortType::GetSingleton();
		break;
	case unsigned_short_type:
		res = UnsignedShortType::GetSingleton();
		break;
	case int_type:
		res = IntType::GetSingleton();
		break;
	case unsigned_int_type:
		res = UnsignedIntType::GetSingleton();
		break;
	case long_type:
		res = LongType::GetSingleton();
		break;
	case unsigned_long_type:
		res = UnsignedLongType::GetSingleton();
		break;
	case long_long_type:
		res = LongLongType::GetSingleton();
		break;
	case unsigned_long_long_type:
		res = UnsignedLongLongType::GetSingleton();
		break;
	case float_type:
		res = FloatType::GetSingleton();
		break;
	case double_type:
		res = DoubleType::GetSingleton();
		break;
	case long_double_type:
		res = LongDoubleType::GetSingleton();
		break;
	default:
		PAF_ASSERT(false);
	}
	return res;
}


BoolType				BoolType::				s_instance("bool");
CharType				CharType::				s_instance("char");
SignedCharType			SignedCharType::		s_instance("signed char");
UnsignedCharType		UnsignedCharType::		s_instance("unsigned char");
WcharType				WcharType::				s_instance("wchar_t");
ShortType				ShortType::				s_instance("short");
UnsignedShortType		UnsignedShortType::		s_instance("unsigned short");
LongType				LongType::				s_instance("long");
UnsignedLongType		UnsignedLongType::		s_instance("unsigned long");
LongLongType			LongLongType::			s_instance("long long");
UnsignedLongLongType	UnsignedLongLongType::	s_instance("unsigned long long");
IntType					IntType::				s_instance("int");
UnsignedIntType			UnsignedIntType::		s_instance("unsigned int");
FloatType				FloatType::				s_instance("float");
DoubleType				DoubleType::			s_instance("double");
LongDoubleType			LongDoubleType::		s_instance("long double");

END_PAF

AUTO_REGISTER_TYPE(paf::BoolType)
AUTO_REGISTER_TYPE(paf::CharType)
AUTO_REGISTER_TYPE(paf::SignedCharType)
AUTO_REGISTER_TYPE(paf::UnsignedCharType)
AUTO_REGISTER_TYPE(paf::WcharType)
AUTO_REGISTER_TYPE(paf::ShortType)
AUTO_REGISTER_TYPE(paf::UnsignedShortType)
AUTO_REGISTER_TYPE(paf::LongType)
AUTO_REGISTER_TYPE(paf::UnsignedLongType)
AUTO_REGISTER_TYPE(paf::LongLongType)
AUTO_REGISTER_TYPE(paf::UnsignedLongLongType)
AUTO_REGISTER_TYPE(paf::IntType)
AUTO_REGISTER_TYPE(paf::UnsignedIntType)
AUTO_REGISTER_TYPE(paf::FloatType)
AUTO_REGISTER_TYPE(paf::DoubleType)
