#include "paflua.h"
#include "../3rd/lua/lua.hpp"
#include "LuaSubclassInvoker.h"
#include "../pafcore/Utility.mh"
#include "../pafcore/Variant.h"
#include "../pafcore/NameSpace.h"
#include "../pafcore/NameSpace.mh"
#include "../pafcore/Metadata.mh"
#include "../pafcore/Type.mh"
#include "../pafcore/ClassType.h"
#include "../pafcore/EnumType.h"
#include "../pafcore/EnumType.mh"
#include "../pafcore/PrimitiveType.h"
#include "../pafcore/Field.h"
#include "../pafcore/Property.h"
#include "../pafcore/Method.h"
#include "../pafcore/Reflection.h"

#include "ArrayFieldWrapper.h"
#include "ArrayPropertyWrapper.h"
#include "CollectionPropertyWrapper.h"

BEGIN_PAFLUA

const char* variant_metatable_name = "paf.Variant";

bool LuaToInt(int& value, lua_State *L, int index)
{
	bool res = false;
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TBOOLEAN: {
		bool b = lua_toboolean(L, index) != 0;
		value = b ? 1 : 0;
		res = true;
		break; }
	case LUA_TNUMBER: {
		value = (int)lua_tointeger(L, index);
		res = true;
		break; }
	case LUA_TUSERDATA: {
		paf::Variant* variant = (paf::Variant*)luaL_checkudata(L, index, variant_metatable_name);
		if (variant && (variant->isValue() || variant->isReference()))
		{
			PAF_ASSERT(false);//unreachable
			res = variant->castToValue(value);
		}
		break; }
	}
	return res;
}

const size_t max_param_count = 20;

paf::ErrorCode GetInstanceProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property)
{
	assert(paf::PropertyCategory::simple_property == property->get_propertyCategory());
	if (nullptr == property->m_simpleGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_simpleGet)(property, that, &value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

paf::ErrorCode SetInstanceProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property)
{
	assert(paf::PropertyCategory::simple_property == property->get_propertyCategory());
	if (nullptr == property->m_simpleSet)
	{
		return paf::ErrorCode::e_property_set_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	paf::ErrorCode errorCode = (*property->m_simpleSet)(property, that, arg);
	return errorCode;
}

paf::ErrorCode GetStaticProperty(lua_State *L, paf::StaticProperty* property)
{
	if (nullptr == property->m_simpleGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_simpleGet)(property, &value);
	if(paf::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

paf::ErrorCode SetStaticProperty(lua_State *L, paf::StaticProperty* property)
{
	if (nullptr == property->m_simpleSet)
	{
		return paf::ErrorCode::e_property_set_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	paf::ErrorCode errorCode = (*property->m_simpleSet)(property, arg);
	return errorCode;
}

paf::ErrorCode GetNestedType(lua_State *L, paf::Type* nestedType)
{
	paf::Variant value;
	value.assignRawPtr(RuntimeTypeOf<paf::Type>::RuntimeType::GetSingleton(), nestedType);
	VariantToLua(L, &value);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetInstanceFieldRef(lua_State *L, paf::Variant* that, paf::InstanceField* field)
{
	//size_t baseOffset;
	//if(!static_cast<paf::ClassType*>(that->m_type)->getClassOffset(baseOffset, field->m_objectType))
	//{
	//	return paf::ErrorCode::e_invalid_type;
	//}
	//size_t fieldAddress = (size_t)that->m_pointer + baseOffset + field->m_offset;
	//paf::Variant value;
	//
	//if(field->isArray())
	//{
	//	value.assignArray(field->m_type, (void*)fieldAddress, field->m_arraySize, field->m_constant, ::paf::Variant::by_array);
	//}
	//else if(field->isPointer())
	//{
	//	value.assignPtr(field->m_type, *(void**)fieldAddress, field->m_constant, ::paf::Variant::by_ref);
	//}
	//else
	//{		
	//	value.assignPtr(field->m_type, (void*)fieldAddress, field->m_type->isPrimitive() ? true : field->m_constant, ::paf::Variant::by_ref);
	//}
	//VariantToLua(L, &value);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode SetInstanceField(lua_State *L, paf::Variant* that, paf::InstanceField* field)
{
	//paf::Variant value;
	//paf::Variant* arg = LuaToVariant(&value, L, 3);
	//paf::Reflection::set

	//if (0 != field->get_arraySize())
	//{
	//	return paf::ErrorCode::e_field_is_an_array;
	//}

	//size_t baseOffset;
	//if(!static_cast<paf::ClassType*>(that->getType())->getClassOffset(baseOffset, field->m_objectType))
	//{
	//	return paf::ErrorCode::e_invalid_object_type;
	//}
	//if (value.getType() != field->getType())
	//{
	//	return paf::ErrorCode::e_invalid_type;
	//}
	//size_t fieldAddress = (size_t)that->getRawPointer() + baseOffset + field->m_offset;
	//field->m_type->copyAssign((void*)fieldAddress, arg->getRawPointer());
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetStaticField(lua_State *L, paf::StaticField* field)
{
	//paf::Variant value;
	//if(field->isArray())
	//{
	//	value.assignArray(field->m_type, (void*)field->m_address, field->m_arraySize, field->m_constant, ::paf::Variant::by_array);
	//}
	//else if (field->isPointer())
	//{
	//	value.assignPtr(field->m_type, *(void**)field->m_address, field->m_constant, ::paf::Variant::by_ref);
	//}
	//else
	//{	
	//	value.assignPtr(field->m_type, (void*)field->m_address, field->m_type->isPrimitive() ? true : field->m_constant, ::paf::Variant::by_ref);
	//}
	//VariantToLua(L, &value);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode SetStaticField(lua_State *L, paf::StaticField* field)
{
	//if (0 != field->get_arraySize())
	//{
	//	return paf::ErrorCode::e_field_is_an_array;
	//}
	//paf::Variant value;
	//paf::Variant* arg = LuaToVariant(&value, L, 3);
	//if (value.getType() != field->getType())
	//{
	//	return paf::ErrorCode::e_invalid_type;
	//}
	//field->m_type->copyAssign((void*)field->get_address(), arg->getRawPointer());
	return paf::ErrorCode::s_ok;
}

int InvokeMethod(lua_State *L, paf::InvokeMethod invoker, int numArgs, int startIndex)
{
	char argumentsBuf[sizeof(paf::Variant)*max_param_count];
	paf::Variant* args[max_param_count];
	if (numArgs > max_param_count)
	{
		numArgs = max_param_count;
	}
	for (int i = 0; i < numArgs; i++)
	{
		paf::Variant* argument = (paf::Variant*)&argumentsBuf[sizeof(paf::Variant)*i];
		new(argument)paf::Variant;
		args[i] = LuaToVariant(argument, L, i + startIndex);
	}
	paf::Variant result;
	paf::ErrorCode errorCode = (*invoker)(&result, args, numArgs);
	for (int i = 0; i < numArgs; i++)
	{
		paf::Variant* argument = (paf::Variant*)&argumentsBuf[sizeof(paf::Variant)*i];
		argument->~Variant();
	}
	if (paf::ErrorCode::s_ok == errorCode)
	{
		if (result.isNull())
		{
			return 0;
		}
		else
		{
			VariantToLua(L, &result);
			return 1;
		}
	}
	RaiseLuaError(L, "", errorCode);
	return 0;
}

int Closure_InvokeMethod(lua_State *L)
{
	paf::InvokeMethod invoker = (paf::InvokeMethod)lua_topointer(L, lua_upvalueindex(1));
	int numArgs = lua_gettop(L);
	return InvokeMethod(L, invoker, numArgs, 1);
}

typedef paf::ErrorCode(paf::Variant::*Variant_New)(paf::Type* type, paf::Variant** args, uint32_t numArgs);

int Type_New(lua_State *L, paf::Type* type, Variant_New function, int numArgs, int startIndex)
{
	char argumentsBuf[sizeof(paf::Variant)*max_param_count];
	paf::Variant* args[max_param_count];
	if (numArgs > max_param_count)
	{
		numArgs = max_param_count;
	}
	for (int i = 0; i < numArgs; i++)
	{
		paf::Variant* argument = (paf::Variant*)&argumentsBuf[sizeof(paf::Variant)*i];
		new(argument)paf::Variant;
		args[i] = LuaToVariant(argument, L, i + startIndex);
	}
	paf::Variant result;
	paf::ErrorCode errorCode = (result.*function)(type, args, numArgs);
	for (int i = 0; i < numArgs; i++)
	{
		paf::Variant* argument = (paf::Variant*)&argumentsBuf[sizeof(paf::Variant)*i];
		argument->~Variant();
	}
	if (paf::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &result);
		return 1;
	}
	RaiseLuaError(L, "", errorCode);
	return 0;
}

int Closure_NewValue(lua_State *L)
{
	paf::Type* type = (paf::Type*)lua_topointer(L, lua_upvalueindex(1));
	int numArgs = lua_gettop(L);
	return Type_New(L, type, &paf::Variant::newValue, numArgs, 1);
}

int Closure_NewSharedPtr(lua_State *L)
{
	paf::Type* type = (paf::Type*)lua_topointer(L, lua_upvalueindex(1));
	int numArgs = lua_gettop(L);
	return Type_New(L, type, &paf::Variant::newSharedPtr, numArgs, 1);
}

int Closure_NewSharedArray(lua_State *L)
{
	paf::Type* type = (paf::Type*)lua_topointer(L, lua_upvalueindex(1));
	int numArgs = lua_gettop(L);
	return Type_New(L, type, &paf::Variant::newSharedArray, numArgs, 1);
}

int Closure_Subclassing(lua_State *L)
{
	paf::ClassType* classType = (paf::ClassType*)lua_topointer(L, lua_upvalueindex(1));
	int numArgs = lua_gettop(L);
	if (1 == numArgs && lua_type(L, -1) == LUA_TTABLE)
	{
		LuaSubclassInvoker* subclassInvoker = new LuaSubclassInvoker(L);
		paf::SharedPtr<paf::Introspectable> implementor = classType->createSubclassProxy(subclassInvoker);
		paf::Variant impVar;
		impVar.assignSharedPtr(std::move(implementor));
		VariantToLua(L, &impVar);
		return 1;
	}
	RaiseLuaError(L, "the argument of _Derive_ must be a table", paf::ErrorCode::e_invalid_arg_type_1);
	return 0;
}

int Variant_Call(lua_State *L)
{
	int numArgs = lua_gettop(L);
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	switch(variant->getType()->getMetaCategory())
	{
	case paf::MetaCategory::instance_method:
		{
			paf::InstanceMethod* method = (paf::InstanceMethod*)variant->getRawPointer();
			int numArgs = lua_gettop(L) - 1;
			return InvokeMethod(L, method->m_invokeMethod, numArgs, 2);
		}
		break;
	case paf::MetaCategory::static_method:
		{
			paf::StaticMethod* method = (paf::StaticMethod*)variant->getRawPointer();
			int numArgs = lua_gettop(L) - 1;
			return InvokeMethod(L, method->m_invokeMethod, numArgs, 2);
	}
		break;
	case paf::MetaCategory::primitive_type:
	case paf::MetaCategory::enumeration_type:
	case paf::MetaCategory::object_type:
		{
			paf::Type* type = (paf::Type*)variant->getRawPointer();
			int numArgs = lua_gettop(L) - 1;
			return Type_New(L, type, &::paf::Variant::newValue, numArgs, 2);
		}
		break;
	//case paf::MetaCategory::object:
	//	{
	//		paf::ClassType* type = (paf::ClassType*)variant->getType();
	//		paf::InstanceMethod* method = type->findInstanceMethod("op_call", true);
	//		if (0 != method)
	//		{
	//			//return InvokeFunction_Operator(L, method->m_invokeMethod);
	//		}
	//	}
	//	break;
	}
	RaiseLuaError(L, "call", paf::ErrorCode::e_is_not_type);
	return 0;
}

int Variant_ToString(lua_State *L)
{
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	PAF_ASSERT(!variant->isNull());
	paf::string_t str;
	if (variant->castToValue(str))
	{
		lua_pushstring(L, str.c_str());
		return 1;
	}
	else
	{
		RaiseLuaError(L, "#", paf::ErrorCode::e_is_not_string);
		return 0;
	}
}

int Variant_Concat(lua_State *L)
{
	size_t len1, len2;
	const char* str1 = luaL_tolstring(L, 1, &len1);
	const char* str2 = luaL_tolstring(L, 2, &len2);
	if (str1 && str2)
	{
		char* buf = (char*)malloc(len1 + len2 + 1);
		memcpy(buf, str1, len1);
		memcpy(buf + len1, str2, len2);
		buf[len1 + len2] = 0;
		lua_pushlstring(L, buf, len1 + len2);
		free(buf);
		return 1;
	}
	else
	{
		RaiseLuaError(L, "..", paf::ErrorCode::e_is_not_string);
		return 0;
	}
}

int Variant_Len(lua_State *L)
{
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	PAF_ASSERT(!variant->isNull());
	if (variant->isArray())
	{
		lua_pushinteger(L, variant->getArraySize());
		return 1;
	}
	else
	{
		RaiseLuaError(L, "#", paf::ErrorCode::e_is_not_array);
		return 0;
	}
}

paf::ErrorCode Variant_Index_Identify(lua_State *L, paf::Variant* variant, const char *name)
{
	switch(variant->getType()->getMetaCategory())
	{
	case paf::MetaCategory::name_space:
		{
			paf::NameSpace* ns = (paf::NameSpace*)variant->getRawPointer();
			paf::Metadata* member = ns->findMember(name);
			if(0 != member)
			{
				paf::Variant value;
				value.assignRawPtr(member);
				VariantToLua(L, &value);
				return paf::ErrorCode::s_ok;
			}
		}
		break;
	case paf::MetaCategory::object_type:
		{
			paf::ClassType* type = (paf::ClassType*)variant->getRawPointer();
			paf::Metadata* member = type->findClassMember(name, true, true);
			if(0 != member)
			{
				paf::Type* memberType = member->getType();
				switch(memberType->getMetaCategory())
				{
				case paf::MetaCategory::static_field:
					return GetStaticField(L, static_cast<paf::StaticField*>(member));
				case paf::MetaCategory::static_property:
					switch (static_cast<paf::StaticProperty*>(member)->get_propertyCategory())
					{
					case paf::PropertyCategory::simple_property:
						return GetStaticProperty(L, static_cast<paf::StaticProperty*>(member));
					case paf::PropertyCategory::array_property:
						return MakeStaticArrayProperty(L, static_cast<paf::StaticProperty*>(member));
					case paf::PropertyCategory::collection_property:
						return MakeStaticMapProperty(L, static_cast<paf::StaticProperty*>(member));
					}
				case paf::MetaCategory::static_method:
					lua_pushlightuserdata(L, static_cast<paf::StaticMethod*>(member)->m_invokeMethod);
					lua_pushcclosure(L, Closure_InvokeMethod, 1);
					return paf::ErrorCode::s_ok;
				case paf::MetaCategory::enumeration_type:
				case paf::MetaCategory::object_type:
					return GetNestedType(L, static_cast<paf::Type*>(member));
				}
			}
		}
		break;
	case paf::MetaCategory::enumeration_type:
		{
			paf::EnumType* et = (paf::EnumType*)variant->getRawPointer();
			paf::Enumerator* enumerator = et->findEnumerator(name);
			if(0 != enumerator)
			{
				paf::Variant value;
				value.assignEnumByInt(et, enumerator->get__value_());
				VariantToLua(L, &value);
				return paf::ErrorCode::s_ok;
			}
		}
		break;
	}
	paf::Metadata* member;
	member = variant->getType()->findMember(name);
	if(0 != member)
	{
		paf::Type* memberType = member->getType();
		switch(memberType->getMetaCategory())
		{
		case paf::MetaCategory::instance_field:
			if (static_cast<paf::InstanceField*>(member)->isArray())
			{
				return MakeInstanceArrayField(L, variant, static_cast<paf::InstanceField*>(member));
			}
			else
			{
				return GetInstanceFieldRef(L, variant, static_cast<paf::InstanceField*>(member));
			}
		case paf::MetaCategory::static_field:
			if (static_cast<paf::StaticField*>(member)->isArray())
			{
				return MakeStaticArrayField(L, static_cast<paf::StaticField*>(member));
			}
			else
			{
				return GetStaticField(L, static_cast<paf::StaticField*>(member));
			}
		case paf::MetaCategory::instance_property:
			switch (static_cast<paf::InstanceProperty*>(member)->get_propertyCategory())
			{
			case paf::PropertyCategory::simple_property:
				return GetInstanceProperty(L, variant, static_cast<paf::InstanceProperty*>(member));
			case paf::PropertyCategory::array_property:
				return MakeInstanceArrayProperty(L, variant, static_cast<paf::InstanceProperty*>(member));
			case paf::PropertyCategory::collection_property:
				return MakeInstanceMapProperty(L, variant, static_cast<paf::InstanceProperty*>(member));
			}
		case paf::MetaCategory::static_property:
			switch (static_cast<paf::StaticProperty*>(member)->get_propertyCategory())
			{
			case paf::PropertyCategory::simple_property:
				return GetStaticProperty(L, static_cast<paf::StaticProperty*>(member));
			case paf::PropertyCategory::array_property:
				return MakeStaticArrayProperty(L, static_cast<paf::StaticProperty*>(member));
			case paf::PropertyCategory::collection_property:
				return MakeStaticMapProperty(L, static_cast<paf::StaticProperty*>(member));
			}
		case paf::MetaCategory::instance_method:
			lua_pushlightuserdata(L, static_cast<paf::InstanceMethod*>(member)->m_invokeMethod);
			lua_pushcclosure(L, Closure_InvokeMethod, 1);
			return paf::ErrorCode::s_ok;
		case paf::MetaCategory::static_method:
			lua_pushlightuserdata(L, static_cast<paf::StaticMethod*>(member)->m_invokeMethod);
			lua_pushcclosure(L, Closure_InvokeMethod, 1);
			return paf::ErrorCode::s_ok;
		case paf::MetaCategory::enumeration_type:
		case paf::MetaCategory::object_type:
			return GetNestedType(L, static_cast<paf::Type*>(member));
		default:
			assert(false);
		}
	}
	else if (name[0] == '_')
	{
		switch (name[1])
		{
		case 'D':
			if (strcmp(&name[2], "erive_") == 0)//_Derive_
			{
				if (paf::MetaCategory::object_type == variant->getType()->getMetaCategory())
				{
					paf::ClassType* classType = (paf::ClassType*)variant->getRawPointer(); 
					lua_pushlightuserdata(L, classType);
					lua_pushcclosure(L, Closure_Subclassing, 1);
					return paf::ErrorCode::s_ok;
				}
				else
				{
					return paf::ErrorCode::e_is_not_class;
				}
			}
			break;
		case 'N':
			if (strncmp(&name[2], "ew",2) == 0)//_New_
			{
				paf::MetaCategory metaCategory = variant->getType()->getMetaCategory();
				if (paf::MetaCategory::primitive_type == metaCategory ||
					paf::MetaCategory::enumeration_type == metaCategory ||
					paf::MetaCategory::object_type == metaCategory)
				{
					paf::Type* type = (paf::Type*)variant->getRawPointer();
					if (strcmp(&name[4], "_") == 0)//_New_
					{
						lua_pushlightuserdata(L, type);
						lua_pushcclosure(L, Closure_NewSharedPtr, 1);
						return paf::ErrorCode::s_ok;
					}
					else if (strcmp(&name[4], "Array_") == 0)//_NewSharedArray_
					{
						lua_pushlightuserdata(L, type);
						lua_pushcclosure(L, Closure_NewSharedArray, 1);
						return paf::ErrorCode::s_ok;
					}
				}
			}
			break;
		case 'a':
			if (strcmp(&name[2], "ddress_") == 0)//_address_
			{
				lua_pushinteger(L, (size_t)variant->getRawPointer());
				return paf::ErrorCode::s_ok;
			}
			break;
		case 'c':
			if (strcmp(&name[2], "ount_") == 0)//_count_
			{
				if (variant->isArray())
				{
					lua_pushinteger(L, variant->getArraySize());
					return paf::ErrorCode::s_ok;
				}
			}
			break;
		case 's':
			if (strcmp(&name[2], "ize_") == 0)//_size_
			{
				lua_pushinteger(L, variant->getType()->get__size_());
				return paf::ErrorCode::s_ok;
			}
			break;
		case 't':
			if (strcmp(&name[2], "ype_") == 0)//_type_
			{
				paf::Variant typeVar;
				typeVar.assignRawPtr(RuntimeTypeOf<paf::Type>::RuntimeType::GetSingleton(), variant->getType());
				VariantToLua(L, &typeVar);
				return paf::ErrorCode::s_ok;
			}
			break;
		}
	}
	return paf::ErrorCode::e_member_not_found;
}

paf::ErrorCode Variant_Index_Subscript(lua_State *L, paf::Variant* variant, size_t index)
{
	paf::Variant item;
	paf::ErrorCode errorCode = variant->subscript(item, index);
	if(paf::ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	VariantToLua(L, &item);
	return paf::ErrorCode::s_ok;
}

int Variant_Index(lua_State *L) 
{
	size_t num;
	const char* str;
	SubscriptCategory sc = Variant_ParseSubscript(num, str, L);

	paf::ErrorCode errorCode = paf::ErrorCode::e_invalid_subscript_type;
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	
	PAF_ASSERT(!variant->isNull());
	
	if (sc_integer == sc)
	{
		errorCode = Variant_Index_Subscript(L, variant, num);
	}
	else if (sc_string == sc)
	{
		errorCode = Variant_Index_Identify(L, variant, str);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	
	if(paf::ErrorCode::s_ok != errorCode)
	{
		char buf[256];
		if (sc_integer == sc)
		{
			sprintf_s(buf, "__index[%zd]", num);
			RaiseLuaError(L, buf, errorCode);
		}
		else if (sc_string == sc)
		{
			sprintf_s(buf, "__index[\"%s\"]", str);
			RaiseLuaError(L, buf, errorCode);
		}
		else
		{
			RaiseLuaError(L, "__index", errorCode);
		}
		return 0;
	}
	return 1;
}

paf::ErrorCode Variant_NewIndex_Identify(lua_State *L, paf::Variant* variant, const char *name)
{
	switch (variant->getType()->getMetaCategory())
	{
	case paf::MetaCategory::object_type:
	{
		paf::ClassType* type = (paf::ClassType*)variant->getRawPointer();
		paf::Metadata* member = type->findClassMember(name, true, true);
		if (0 != member)
		{
			paf::Type* memberType = member->getType();
			switch (memberType->getMetaCategory())
			{
			case paf::MetaCategory::static_field:
				return SetStaticField(L, static_cast<paf::StaticField*>(member));
			case paf::MetaCategory::static_property:
				return SetStaticProperty(L, static_cast<paf::StaticProperty*>(member));
			}
		}
	}
	break;
	}
	paf::Metadata* member;
	member = variant->getType()->findMember(name);
	if (0 != member)
	{
		paf::Type* memberType = member->getType();
		switch (memberType->getMetaCategory())
		{
		case paf::MetaCategory::instance_field:
			return SetInstanceField(L, variant, static_cast<paf::InstanceField*>(member));
		case paf::MetaCategory::static_field:
			return SetStaticField(L, static_cast<paf::StaticField*>(member));
		case paf::MetaCategory::instance_property:
			return SetInstanceProperty(L, variant, static_cast<paf::InstanceProperty*>(member));
		case paf::MetaCategory::static_property:
			return SetStaticProperty(L, static_cast<paf::StaticProperty*>(member));
		}
	}
	return paf::ErrorCode::e_member_not_found;
}

paf::ErrorCode Variant_NewIndex_Subscript(lua_State *L, paf::Variant* variant, size_t index)
{
	paf::Variant item;
	paf::ErrorCode errorCode = variant->subscript(item, index);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	PAF_ASSERT(item.isReference());
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	return errorCode;
}

int Variant_NewIndex(lua_State *L)
{
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	size_t num;
	const char* str;
	SubscriptCategory sc = Variant_ParseSubscript(num, str, L);

	paf::ErrorCode errorCode = paf::ErrorCode::e_invalid_subscript_type;
	if(sc_integer == sc)
	{
		errorCode = Variant_NewIndex_Subscript(L, variant, num);
	}
	else if(sc_string == sc)
	{
		errorCode = Variant_NewIndex_Identify(L, variant, str);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if(paf::ErrorCode::s_ok != errorCode)
	{
		char buf[256];
		if (sc_integer == sc)
		{
			sprintf_s(buf, "__newindex[%zd]", num);
			RaiseLuaError(L, buf, errorCode);
		}
		else if (sc_string == sc)
		{
			sprintf_s(buf, "__newindex[\"%s\"]", str);
			RaiseLuaError(L, buf, errorCode);
		}
		else
		{
			RaiseLuaError(L, "__newindex", errorCode);
		}
		return 0;
	}
	return 1;
}

int Variant_GC(lua_State *L)
{
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	variant->~Variant();
	return 0;
}

struct luaL_Reg g_variant_reg [] = 
{
	{ "__gc", Variant_GC },
	{ "__index", Variant_Index },
	{ "__newindex", Variant_NewIndex },
	{ "__tostring", Variant_ToString},
	{ "__concat", Variant_Concat},
	{ "__call", Variant_Call },
	{ "__len", Variant_Len },
	{NULL, NULL}
};

END_PAFLUA
