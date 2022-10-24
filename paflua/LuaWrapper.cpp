#include "paflua.h"
#include "../3rd/lua/src/lua.hpp"
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
#include "../pafcore/InstanceField.h"
#include "../pafcore/StaticField.h"
#include "../pafcore/InstanceProperty.h"
#include "../pafcore/StaticProperty.h"
#include "../pafcore/InstanceMethod.h"
#include "../pafcore/StaticMethod.h"
#include "../pafcore/Enumerator.h"
#include "../pafcore/PrimitiveType.h"
#include "../pafcore/String.h"
#include "../pafcore/String.mh"
#include "../pafcore/Iterator.h"

#include <new.h>
#include <string.h>
#include <assert.h>
#include <Windows.h>


BEGIN_PAFLUA

static void DebugTrace(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int iBuf;
	char szBuffer[512];
	iBuf = vsprintf_s(szBuffer, format, args);
	OutputDebugStringA(szBuffer);
	va_end(args);
}


void stackDump (lua_State *L) 
{
	int i;
	int top = lua_gettop(L);
	DebugTrace("stackDump %d\n", top);

	for (i = 1; i <= top; i++) {
		int t = lua_type(L, i);
		switch (t) {
		case LUA_TSTRING:
			DebugTrace("string: %s", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			DebugTrace(lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			if(lua_isinteger(L, i))
			{
				DebugTrace("integer: %lld", lua_tointeger(L, i));
			}
			else
			{
				DebugTrace("number: %g", lua_tonumber(L, i));
			}
			break;
		case LUA_TUSERDATA:
			DebugTrace("userdata: %s", lua_tostring(L, i));
			break;
		default:
			DebugTrace("other: %s", lua_typename(L, t));
			break;
		}
		DebugTrace("\n");
	}
	DebugTrace("end stackDump\n");
}


const size_t max_param_count = 20;
const char* variant_metatable_name = "paf.Variant";
const char* instanceArrayProperty_metatable_name = "paf.InstanceArrayProperty";
const char* staticArrayProperty_metatable_name = "paf.StaticArrayProperty";
const char* instanceMapProperty_metatable_name = "paf.InstanceMapProperty";
const char* staticMapProperty_metatable_name = "paf.StaticMapProperty";

enum class CompareOperation
{
	less_than,
	equal_to,
	less_equal,
};

static const char* s_compareOperationName[] =
{
	"op_less",
	"op_equal",
	"op_lessEqual",
};

struct InstancePropertyInstance
{
	paf::InstanceProperty* property;
	paf::Variant* object;
};

struct StaticPropertyInstance
{
	paf::StaticProperty* property;
};

void Variant_Error(lua_State *L, const char* name, paf::ErrorCode errorCode)
{
	luaL_error(L, "idlcpp error: %s, Error: %d, %s\n", name, errorCode, paf::ErrorCodeToString(errorCode));
}

int Variant_GC(lua_State *L) 
{
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	variant->~Variant();
	return 0;
}


paf::ErrorCode GetPrimitiveOrEnum(lua_State *L, paf::Variant* variant)
{
	if (variant->getType()->isPrimitive())
	{
		paf::PrimitiveType* primitiveType = static_cast<paf::PrimitiveType*>(variant->getType());
		switch (primitiveType->getPrimitiveTypeCategory())
		{
		case paf::float_type:
		case paf::double_type: {
			lua_Number value;
			variant->castToPrimitive<lua_Number>(value);
			lua_pushnumber(L, value);
			break; }
		case paf::bool_type: {
			bool value;
			variant->castToPrimitive<bool>(value);
			lua_pushboolean(L, value ? 1 : 0);
			break; }
		default: {
			lua_Integer value;
			variant->castToPrimitive<lua_Integer>(value);
			lua_pushinteger(L, value);
			break; }
		}
		return paf::ErrorCode::s_ok;
	}
	else if (variant->getType()->isEnumeration())
	{
		lua_Integer value;
		variant->castToPrimitive<lua_Integer>(value);
		lua_pushinteger(L, value);
		return paf::ErrorCode::s_ok;
	}
	return paf::ErrorCode::e_invalid_type;
}

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
		if (variant && !variant->isNull())
		{
			res = variant->castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &value);
		}
		break;}
	}
	return res;
}

paf::Variant* LuaToVariant(paf::Variant* value, lua_State *L, int index)
{
	paf::Variant* res = value;
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TNIL:
		value->clear();
		break;
	case LUA_TSTRING:
		{
			size_t len;
			const char* s = lua_tolstring(L, index, &len);
			value->makeValue<paf::String>(s);
		}
		break;
	case LUA_TBOOLEAN:
		{
			bool b = lua_toboolean(L, index) != 0;
			value->assignValue<bool>(b);
		}
		break;
	case LUA_TNUMBER:
		if(lua_isinteger(L, index))
		{
			lua_Integer i = lua_tointeger(L, index);
			value->assignValue<lua_Integer>(i);
		}
		else
		{
			lua_Number n = lua_tonumber(L, index);
			value->assignValue<lua_Number>(n);
		}
		break;
	case LUA_TUSERDATA:
		{
			paf::Variant* variant = (paf::Variant*)luaL_checkudata(L, index, variant_metatable_name); 
			if(variant)
			{
				res = variant;
			}
		}
		break;
	}
	return res;
}

void VariantToLua(lua_State *L, paf::Variant* variant)
{
	if (variant->isNull())
	{
		lua_pushnil(L);
		return;
	}
	if (variant->isValue())
	{
		paf::Type* type = variant->getType();
		if (type->isPrimitive())
		{
			paf::PrimitiveType* primitiveType = static_cast<paf::PrimitiveType*>(type);
			switch (primitiveType->getPrimitiveTypeCategory())
			{
			case paf::float_type:
			case paf::double_type: {
				lua_Number value;
				variant->castToPrimitive<lua_Number>(value);
				lua_pushnumber(L, value);
				break; }
			case paf::bool_type: {
				bool value;
				variant->castToPrimitive<bool>(value);
				lua_pushboolean(L, value ? 1 : 0);
				break; }
			default: {
				lua_Integer value;
				variant->castToPrimitive<lua_Integer>(value);
				lua_pushinteger(L, value);
				break; }
			}
			return;
		}
		else if (type->isEnumeration())
		{
			paf::EnumType* enumType = static_cast<paf::EnumType*>(type);
			lua_Integer value;
			enumType->castToPrimitive(&value, RuntimeTypeOf<lua_Integer>::RuntimeType::GetSingleton(), variant->getRawPointer());
			lua_pushinteger(L, value);
			return;
		}
		else
		{
			paf::ClassType* classType = static_cast<paf::ClassType*>(type);
			if (paf::string_t::GetType() == classType)
			{
				paf::string_t* str = static_cast<paf::string_t*>(variant->getRawPointer());
				lua_pushstring(L, str->c_str());
				return;
			}
			else
			{
				size_t offset;
				if (classType->getClassOffset(offset, paf::StringBase::GetType()))
				{
					paf::StringBase* str = (paf::StringBase*)((size_t)variant->getRawPointer() + offset);
					lua_pushstring(L, str->toString());
					return;
				}
			}
		}
	}
	void* p = lua_newuserdata(L, sizeof(paf::Variant));
	paf::Variant* res = new(p)paf::Variant(std::move(*variant));
	luaL_getmetatable(L, variant_metatable_name);
	lua_setmetatable(L, -2);
}

int InvokeFunction(lua_State *L, paf::FunctionInvoker invoker, int numArgs, int startIndex)
{
	char argumentsBuf[sizeof(paf::Variant)*max_param_count];
	paf::Variant* args[max_param_count]; 
	if(numArgs > max_param_count)
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
	if(paf::ErrorCode::s_ok == errorCode)
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
	Variant_Error(L, "", errorCode);
	return 0;
}

int InvokeFunction_Method(lua_State *L, paf::FunctionInvoker invoker)
{
	int numArgs = lua_gettop(L) - 1;
	return InvokeFunction(L, invoker, numArgs, 2);
}

int InvokeFunction_Operator(lua_State *L, paf::FunctionInvoker invoker)
{
	int numArgs = lua_gettop(L);
	return InvokeFunction(L, invoker, numArgs, 1);
}

int InvokeFunction_ComparisonOperator(lua_State *L, paf::FunctionInvoker invoker)
{
	int numArgs = lua_gettop(L);
	paf::Variant arguments[max_param_count];
	paf::Variant* args[max_param_count]; 
	if(numArgs > max_param_count)
	{
		numArgs = max_param_count;
	}
	for (int i = 0; i < numArgs; i++)
	{
		args[i] = LuaToVariant(&arguments[i], L, i + 1);
	}
	paf::Variant result;
	paf::ErrorCode errorCode = (*invoker)(&result, args, numArgs);
	if(paf::ErrorCode::s_ok == errorCode)
	{
		bool b;
		if (result.castToPrimitive<bool>(b))
		{
			lua_pushboolean(L, b ? 1 : 0);
		}
		else
		{
			VariantToLua(L, &result);
		}
		return 1;
	}
	Variant_Error(L, "", errorCode);
	return 0;
}

int FunctionInvoker_Closure(lua_State *L)
{
	const void* p = lua_topointer(L, lua_upvalueindex(1));
	paf::FunctionInvoker invoker = (paf::FunctionInvoker)p;
	int numArgs = lua_gettop(L);
	return InvokeFunction(L, invoker, numArgs, 1);
}

int FunctionInvoker_Clone(lua_State *L, paf::FunctionInvoker invoker)
{
	return InvokeFunction(L, invoker, 1, 1);
}

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

paf::ErrorCode GetInstanceArrayProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property, size_t index)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());

	if (0 == property->m_arrayGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_arrayGet)(property, that, index, &value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

paf::ErrorCode SetInstanceArrayProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property, size_t index)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());
	if (0 == property->m_arraySet)
	{
		return paf::ErrorCode::e_property_set_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	paf::ErrorCode errorCode = (*property->m_arraySet)(property, that, index, arg);
	return errorCode;
}

paf::ErrorCode GetInstanceArrayPropertySize(lua_State *L, paf::Variant* that, paf::InstanceProperty* property)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());
	if (0 == property->m_arraySize)
	{
		return paf::ErrorCode::e_property_size_not_implemented;
	}
	size_t size;
	paf::ErrorCode errorCode = (*property->m_arraySize)(property, that, size);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		lua_pushinteger(L, size);
	}
	return errorCode;
}

paf::ErrorCode MakeInstanceArrayProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());

	void* p = lua_newuserdata(L, sizeof(InstancePropertyInstance));
	((InstancePropertyInstance*)p)->property = property;
	((InstancePropertyInstance*)p)->object = that;
	luaL_getmetatable(L, instanceArrayProperty_metatable_name);
	lua_setmetatable(L, -2);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetInstanceMapProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property, paf::Iterator* iterator)
{
	assert(paf::PropertyCategory::collection_property == property->get_propertyCategory());

	if (0 == property->m_collectionGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_collectionGet)(property, that, iterator, &value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

paf::ErrorCode SetInstanceMapProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property, paf::Iterator* iterator)
{
	assert(paf::PropertyCategory::collection_property == property->get_propertyCategory());

	if (0 == property->m_collectionSet)
	{
		return paf::ErrorCode::e_property_set_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	paf::ErrorCode errorCode = (*property->m_collectionSet)(property, that, iterator, 1, arg);
	return errorCode;
}

paf::ErrorCode MakeInstanceMapProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property)
{
	assert(paf::PropertyCategory::collection_property == property->get_propertyCategory());

	void* p = lua_newuserdata(L, sizeof(InstancePropertyInstance));
	((InstancePropertyInstance*)p)->property = property;
	((InstancePropertyInstance*)p)->object = that;
	luaL_getmetatable(L, instanceMapProperty_metatable_name);
	lua_setmetatable(L, -2);
	return paf::ErrorCode::s_ok;
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

paf::ErrorCode GetStaticArrayProperty(lua_State *L, paf::StaticProperty* property, size_t index)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());

	if (0 == property->m_arrayGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_arrayGet)(property, index, &value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

paf::ErrorCode SetStaticArrayProperty(lua_State *L, paf::StaticProperty* property, size_t index)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());
	if (0 == property->m_arraySet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	paf::ErrorCode errorCode = (*property->m_arraySet)(property, index, arg);
	return errorCode;
}

paf::ErrorCode GetStaticArrayPropertySize(lua_State *L, paf::StaticProperty* property)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());
	if (0 == property->m_arraySize)
	{
		return paf::ErrorCode::e_property_size_not_implemented;
	}
	size_t size;
	paf::ErrorCode errorCode = (*property->m_arraySize)(property, size);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		lua_pushinteger(L, size);
	}
	return errorCode;
}

paf::ErrorCode MakeStaticArrayProperty(lua_State *L, paf::StaticProperty* property)
{
	assert(paf::PropertyCategory::array_property == property->get_propertyCategory());

	void* p = lua_newuserdata(L, sizeof(StaticPropertyInstance));
	((StaticPropertyInstance*)p)->property = property;
	luaL_getmetatable(L, staticArrayProperty_metatable_name);
	lua_setmetatable(L, -2);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetStaticMapProperty(lua_State *L, paf::StaticProperty* property, paf::Iterator* iterator)
{
	assert(paf::PropertyCategory::collection_property == property->get_propertyCategory());

	if (0 == property->m_collectionGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_collectionGet)(property, iterator, &value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		VariantToLua(L, &value);
	}
	return errorCode;
}

paf::ErrorCode SetStaticMapProperty(lua_State *L, paf::StaticProperty* property, paf::Iterator* iterator)
{
	assert(paf::PropertyCategory::collection_property == property->get_propertyCategory());

	if (0 == property->m_collectionSet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	paf::ErrorCode errorCode = (*property->m_collectionSet)(property, iterator, 1, arg);
	return errorCode;
}

paf::ErrorCode MakeStaticMapProperty(lua_State *L, paf::StaticProperty* property)
{
	assert(paf::PropertyCategory::collection_property == property->get_propertyCategory());

	void* p = lua_newuserdata(L, sizeof(StaticPropertyInstance));
	((StaticPropertyInstance*)p)->property = property;
	luaL_getmetatable(L, staticMapProperty_metatable_name);
	lua_setmetatable(L, -2);
	return paf::ErrorCode::s_ok;
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
	if (0 != field->get_arraySize())
	{
		return paf::ErrorCode::e_field_is_an_array;
	}

	size_t baseOffset;
	if(!static_cast<paf::ClassType*>(that->getType())->getClassOffset(baseOffset, field->m_objectType))
	{
		return paf::ErrorCode::e_invalid_object_type;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	if (value.getType() != field->getType())
	{
		return paf::ErrorCode::e_invalid_type;
	}
	size_t fieldAddress = (size_t)that->getRawPointer() + baseOffset + field->m_offset;
	field->m_type->copyAssign((void*)fieldAddress, arg->getRawPointer());
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
	if (0 != field->get_arraySize())
	{
		return paf::ErrorCode::e_field_is_an_array;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	if (value.getType() != field->getType())
	{
		return paf::ErrorCode::e_invalid_type;
	}
	field->m_type->copyAssign((void*)field->get_address(), arg->getRawPointer());
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode SetArraySize(lua_State *L, paf::Variant* that)
{
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);
	if (!arg->castToPrimitive(RuntimeTypeOf<size_t>::RuntimeType::GetSingleton(), &that->m_arraySize))
	{
		return paf::ErrorCode::e_invalid_property_type;
	}
	return paf::ErrorCode::s_ok;
}


paf::ErrorCode SetPrimitiveOrEnum(lua_State *L, paf::Variant* variant)
{
	//assert(variant->getType()->isPrimitive() || variant->getType()->isEnumeration());

	//paf::Variant value;
	//paf::Variant* arg = LuaToVariant(&value, L, 3);
	//arg->castto
	//if (!arg->castToObject(variant->m_type, (void*)variant->m_pointer))
	//{
	//	return paf::ErrorCode::e_invalid_property_type;
	//}
	return paf::ErrorCode::s_ok;
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
			return InvokeFunction_Method(L, method->m_invoker);
		}
		break;
	case paf::MetaCategory::static_method:
		{
			paf::StaticMethod* method = (paf::StaticMethod*)variant->getRawPointer();
			return InvokeFunction_Method(L, method->m_invoker);
		}
		break;
	case paf::MetaCategory::primitive_type:
		{
			paf::PrimitiveType* type = (paf::PrimitiveType*)variant->getRawPointer();
			assert(strcmp(type->getStaticMethod(0)->m_name, "New") == 0);
			return InvokeFunction_Method(L, type->getStaticMethod(0)->m_invoker);
		}
		break;
	case paf::MetaCategory::object_type:
		{
			paf::ClassType* type = (paf::ClassType*)variant->getRawPointer();
			paf::StaticMethod* method = type->findStaticMethod("New", false);
			if(0 != method)
			{
				return InvokeFunction_Method(L, method->m_invoker);
			}
		}
		break;
	case paf::MetaCategory::object:
		{
			paf::ClassType* type = (paf::ClassType*)variant->getType();
			paf::InstanceMethod* method = type->findInstanceMethod("op_call", true);
			if (0 != method)
			{
				return InvokeFunction_Operator(L, method->m_invoker);
			}
		}
		break;
	}
	Variant_Error(L, "call", paf::ErrorCode::e_is_not_type);
	return 0;
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
		Variant_Error(L, "#", paf::ErrorCode::e_is_not_array);
		return 0;
	}
}

int GetEnumerator(lua_State *L, paf::Enumerator* enumerator)
{
	paf::Variant value;
	value.assignEnumByInt(enumerator->get__type_(), enumerator->get__value_());
	VariantToLua(L, &value);
	return 1;
}

inline bool isNumberString(const char* str)
{
	char c;
	while(c = *str++)
	{
		if(c < '0' || '9' < c)
		{
			return false;
		}
	}
	return true;
}

int Variant_ComparisonOperator(lua_State *L, CompareOperation op)
{
	paf::Variant* variant = (paf::Variant*)luaL_checkudata(L, 1, variant_metatable_name);
	PAF_ASSERT(0 != variant);
	switch (variant->getType()->getMetaCategory())
	{
	case paf::MetaCategory::primitive: {
		paf::PrimitiveType* type = (paf::PrimitiveType*)variant->getType();
		//paf::InstanceMethod* method = type->findInstanceMethod(s_compareOperationName[(int)op]);
		//if (0 != method)
		//{
		//	return InvokeFunction_ComparisonOperator(L, method->m_invoker);
		//}
		break; }
	case paf::MetaCategory::enumeration: {
		int lhs, rhs;
		if(variant->castToPrimitive(RuntimeTypeOf<int>::RuntimeType::GetSingleton(), &lhs)
			&& LuaToInt(rhs, L, 2))
		{ 
			bool compareResult = false;
			switch (op)
			{
			case CompareOperation::less_than:
				compareResult = lhs < rhs;
				break;
			case CompareOperation::less_equal:
				compareResult = lhs <= rhs;
				break;
			default:
				compareResult = lhs == rhs;
				break;
			}
			lua_pushboolean(L, compareResult ? 1 : 0);
			return 1;
		}
		else
		{
			Variant_Error(L, s_compareOperationName[(int)op], paf::ErrorCode::e_invalid_type);
			return 0;
		}
		break; }
	case paf::MetaCategory::object:{
		paf::ClassType* type = (paf::ClassType*)variant->getType();
		paf::InstanceMethod* method = type->findInstanceMethod(s_compareOperationName[(int)op], true);
		if (0 != method)
		{
			return InvokeFunction_ComparisonOperator(L, method->m_invoker);
		}
		break; }
	}

	Variant_Error(L, s_compareOperationName[(int)op], paf::ErrorCode::e_member_not_found);
	return 0;
}

int Variant_Less(lua_State *L)
{
	return Variant_ComparisonOperator(L, CompareOperation::less_than);
}

int Variant_LessEqual(lua_State *L)
{
	return Variant_ComparisonOperator(L, CompareOperation::less_equal);
}

int Variant_Equal(lua_State *L)
{
	return Variant_ComparisonOperator(L, CompareOperation::equal_to);
}

int Subclassing(lua_State *L)
{
	const void* p = lua_topointer(L, lua_upvalueindex(1));
	paf::ClassType* classType = (paf::ClassType*)p;
	int numArgs = lua_gettop(L);
	if (1 == numArgs && lua_type(L, -1) == LUA_TTABLE)
	{
		LuaSubclassInvoker* subclassInvoker = new LuaSubclassInvoker(L);
		paf::UniquePtr<paf::Introspectable> implementor = classType->createSubclassProxy(subclassInvoker);
		paf::Variant impVar;
		impVar.assignUniquePtr(std::move(implementor));
		VariantToLua(L, &impVar);
		return 1;
	}
	Variant_Error(L, "the argument of _Derive_ must be a table", paf::ErrorCode::e_invalid_arg_type_1);
	return 0;
}

//int Delegate_AddCallBack(lua_State *L)
//{
//	const void* p = lua_topointer(L, lua_upvalueindex(1));
//	paf::Delegate* delegate = (paf::Delegate*)p;
//	int numArgs = lua_gettop(L);
//	if (1 == numArgs)
//	{
//		if (lua_type(L, -1) != LUA_TFUNCTION)
//		{
//			Variant_Error(L, "the second argument of _delegate_ must be a function", paf::ErrorCode::e_invalid_arg_type_1);
//			return 0;
//		}		
//		LuaCallBack2* callBack = new LuaCallBack2(L);
//		delegate->addCallBack(callBack);
//		paf::Variant impVar;
//		impVar.assignReferencePtr(RuntimeTypeOf<::paflua::LuaCallBack>::RuntimeType::GetSingleton(), callBack, false, ::paf::Variant::by_new_ptr);
//		impVar.setSubClassProxy();
//		VariantToLua(L, &impVar);
//		return 1;
//	}
//	else if(2 == numArgs)
//	{
//		if (lua_type(L, -2) != LUA_TTABLE)
//		{
//			Variant_Error(L, "the first argument of _delegate_ must be a table", paf::ErrorCode::e_invalid_arg_type_1);
//			return 0;
//		}
//		if (!lua_isstring(L, -1))
//		{
//			Variant_Error(L, "the second argument of _delegate_ must be a string", paf::ErrorCode::e_invalid_arg_type_2);
//			return 0;
//		}
//		const char* str = lua_tostring(L, -1);
//		LuaCallBack* callBack = new LuaCallBack(L, str);
//		delegate->addCallBack(callBack);
//		paf::Variant impVar;
//		impVar.assignReferencePtr(RuntimeTypeOf<::paflua::LuaCallBack>::RuntimeType::GetSingleton(), callBack, false, ::paf::Variant::by_new_ptr);
//		impVar.setSubClassProxy();
//		VariantToLua(L, &impVar);
//		return 1;
//	}
//	else
//	{
//		Variant_Error(L, "the argument count of _delegate_ must be 1 or 2", paf::ErrorCode::e_invalid_arg_num);
//		return 0;
//	}
//}

//int Delegate_RemoveCallBack(lua_State *L)
//{
//	const void* p = lua_topointer(L, lua_upvalueindex(1));
//	paf::Delegate* delegate = (paf::Delegate*)p;
//	int numArgs = lua_gettop(L);
//	if (numArgs != 1)
//	{
//		Variant_Error(L, "the argument count of _delegate_ must be 1", paf::ErrorCode::e_invalid_arg_num);
//		return 0;
//	}
//	int type = lua_type(L, index);
//	if (LUA_TUSERDATA == type)
//	{
//		paf::Variant* variant = (paf::Variant*)luaL_checkudata(L, -1, variant_metatable_name);
//		if (variant)
//		{
//			if (paf::reference_object == variant->m_type->m_category &&
//				static_cast<paf::ClassType*>(variant->m_type)->isType(RuntimeTypeOf<::paf::Lu>::RuntimeType::GetSingleton()))
//
//		}
//	}
//	if(0)
//	case LUA_TSTRING:
//	case :
//	{
//	}
//	break;
//
//
//	paf::Variant value;
//	paf::Variant* arg = LuaToVariant(&value, L, 3);
//
//	LuaToVariant()
//	if (lua_type(L, -2) != LUA_TTABLE)
//	{
//		Variant_Error(L, "the first argument of _delegate_ must be a table", paf::ErrorCode::e_invalid_arg_type_1);
//		return 0;
//	}
//	if (!lua_isstring(L, -1))
//	{
//		Variant_Error(L, "the second argument of _delegate_ must be a string", paf::ErrorCode::e_invalid_arg_type_2);
//		return 0;
//	}
//	const char* str = lua_tostring(L, -1);
//	LuaCallBack* callBack = new LuaCallBack(L, str);
//	delegate->addCallBack(callBack);
//	paf::Variant impVar;
//	impVar.assignReferencePtr(RuntimeTypeOf<::paflua::LuaCallBack>::RuntimeType::GetSingleton(), callBack, false, ::paf::Variant::by_new_ptr);
//	impVar.setSubClassProxy();
//	VariantToLua(L, &impVar);
//	return 1;
//}

//int CastPtr(lua_State *L)
//{
//	const void* p = lua_topointer(L, lua_upvalueindex(1));
//	paf::Type* type = (paf::Type*)p;
//	int numArgs = lua_gettop(L);
//	if (1 == numArgs)
//	{
//		paf::Variant value;
//		paf::Variant* arg = LuaToVariant(&value, L, 1);
//		paf::Variant dstPtr;
//		arg->reinterpretCastToPtr(dstPtr, type);
//		VariantToLua(L, &dstPtr);
//		return 1;
//	}
//	Variant_Error(L, "_CastPtr_", paf::ErrorCode::e_invalid_arg_type_1);
//	return 0;
//}

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
				value.assignRawPtr<paf::Metadata>(member);
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
					lua_pushlightuserdata(L, static_cast<paf::StaticMethod*>(member)->m_invoker);
					lua_pushcclosure(L, FunctionInvoker_Closure, 1);
					return paf::ErrorCode::s_ok;
				//case paf::static_method:
				//	{
				//		paf::Variant value;
				//		value.assignReferencePtr(RuntimeTypeOf<paf::Metadata>::RuntimeType::GetSingleton(), member, false, ::paf::Variant::by_ptr);
				//		VariantToLua(L, &value);
				//		return paf::ErrorCode::s_ok;
				//	}
				case paf::MetaCategory::enumeration_type:
				case paf::MetaCategory::object_type:
					return GetNestedType(L, static_cast<paf::Type*>(member));
				}
			}
		}
		break;
	case paf::MetaCategory::primitive_type:
		{
			paf::PrimitiveType* type = (paf::PrimitiveType*)variant->getRawPointer();
			paf::Metadata* member = type->findTypeMember(name);
			if(0 != member)
			{
				paf::Type* memberType = member->getType();
				switch(memberType->getMetaCategory())
				{
				case paf::MetaCategory::static_method:
					lua_pushlightuserdata(L, static_cast<paf::StaticMethod*>(member)->m_invoker);
					lua_pushcclosure(L, FunctionInvoker_Closure, 1);
					return paf::ErrorCode::s_ok;
				//case paf::static_method:
				//	{
				//		paf::Variant value;
				//		value.assignReferencePtr(RuntimeTypeOf<paf::Metadata>::RuntimeType::GetSingleton(), member, false, ::paf::Variant::by_ptr);
				//		VariantToLua(L, &value);
				//		return paf::ErrorCode::s_ok;
				//	}
				default:
					assert(false);
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
			return GetInstanceFieldRef(L, variant, static_cast<paf::InstanceField*>(member));
		case paf::MetaCategory::static_field:
			return GetStaticField(L, static_cast<paf::StaticField*>(member));
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
			lua_pushlightuserdata(L, static_cast<paf::InstanceMethod*>(member)->m_invoker);
			lua_pushcclosure(L, FunctionInvoker_Closure, 1);
			return paf::ErrorCode::s_ok;
		case paf::MetaCategory::static_method:
			lua_pushlightuserdata(L, static_cast<paf::StaticMethod*>(member)->m_invoker);
			lua_pushcclosure(L, FunctionInvoker_Closure, 1);
			return paf::ErrorCode::s_ok;
		//case paf::static_method:
		//case paf::instance_method:
		//	{
		//		paf::Variant value;
		//		value.assignReferencePtr(RuntimeTypeOf<paf::Metadata>::RuntimeType::GetSingleton(), member, false, ::paf::Variant::by_ptr);
		//		VariantToLua(L, &value);
		//		return paf::ErrorCode::s_ok;
		//	}
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
		case '\0':			
			if (variant->getType()->isPrimitive() || variant->getType()->isEnumeration())//_
			{
				return GetPrimitiveOrEnum(L, variant);
			}
			break;
		//case 'C':
		//	if (strcmp(&name[2], "astPtr_") == 0)//_CastPtr_
		//	{
		//		if (paf::void_type == variant->m_type->m_category ||
		//			paf::primitive_type == variant->m_type->m_category ||
		//			paf::enum_type == variant->m_type->m_category ||
		//			paf::class_type == variant->m_type->m_category)
		//		{
		//			paf::Type* type = (paf::Type*)variant->m_pointer;
		//			lua_pushlightuserdata(L, type);
		//			lua_pushcclosure(L, CastPtr, 1);
		//			return paf::ErrorCode::s_ok;
		//		}
		//		else
		//		{
		//			return paf::ErrorCode::e_is_not_type;
		//		}
		//	}
		//	break;
		case 'D':
			if (strcmp(&name[2], "erive_") == 0)//_Derive_
			{
				if (paf::MetaCategory::object_type == variant->getType()->getMetaCategory())
				{
					paf::ClassType* classType = (paf::ClassType*)variant->getRawPointer();
					lua_pushlightuserdata(L, classType);
					lua_pushcclosure(L, Subclassing, 1);
					return paf::ErrorCode::s_ok;
				}
				else
				{
					return paf::ErrorCode::e_is_not_class;
				}
			}
			break;
		case 'a':
			//if (strcmp(&name[2], "dd_") == 0)//_Derive_
			//{
			//	if (paf::value_object == variant->m_type->m_category &&
			//		static_cast<paf::ClassType*>(variant->m_type)->isType(RuntimeTypeOf<::paf::Delegate>::RuntimeType::GetSingleton()))
			//	{
			//		lua_pushlightuserdata(L, variant->m_pointer);
			//		lua_pushcclosure(L, Delegate_AddCallBack, 1);
			//		return paf::ErrorCode::s_ok;
			//	}
			//	else
			//	{
			//		return paf::ErrorCode::e_is_not_class;
			//	}
			//}
			//else 
			if (strcmp(&name[2], "ddress_") == 0)//_address_
			{
				lua_pushinteger(L, (size_t)variant->getRawPointer());
				return paf::ErrorCode::s_ok;
			}
			break;
		case 'c':
			if (strcmp(&name[2], "ount_") == 0)//_count_
			{
				lua_pushinteger(L, variant->getArraySize());
				return paf::ErrorCode::s_ok;
			}
			break;
		case 'i':
			if (strcmp(&name[2], "sNullPtr_") == 0)//_isNullPtr_
			{
				lua_pushboolean(L, 0 == variant->isNull() ? 1 : 0);
				return paf::ErrorCode::s_ok;
			}
			break;
		//case 'n':
		//	if (strcmp(&name[2], "ullPtr_") == 0)//_nullPtr_
		//	{
		//		if (paf::void_type == variant->m_type->m_category ||
		//			paf::primitive_type == variant->m_type->m_category ||
		//			paf::enum_type == variant->m_type->m_category ||
		//			paf::class_type == variant->m_type->m_category)
		//		{
		//			paf::Type* type = (paf::Type*)variant->m_pointer;
		//			paf::Variant var;
		//			var.assignPtr(type, 0, false, paf::Variant::by_ptr);
		//			VariantToLua(L, &var);
		//			return paf::ErrorCode::s_ok;
		//		}
		//		else
		//		{
		//			return paf::ErrorCode::e_is_not_type;
		//		}
		//	}
		//	break;
		//case 'r':
		//	if (strcmp(&name[2], "emove_") == 0)//_remove_
		//	{
		//		if (paf::value_object == variant->m_type->m_category &&
		//			static_cast<paf::ClassType*>(variant->m_type)->isType(RuntimeTypeOf<::paf::Delegate>::RuntimeType::GetSingleton()))
		//		{
		//			lua_pushlightuserdata(L, variant->m_pointer);
		//			lua_pushcclosure(L, Delegate_RemoveCallBack, 1);
		//			return paf::ErrorCode::s_ok;
		//		}
		//		else
		//		{
		//			return paf::ErrorCode::e_is_not_class;
		//		}
		//	}
		//	break;
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

paf::ErrorCode Variant_NewIndex_Identify(lua_State *L, paf::Variant* variant, const char *name)
{
	switch(variant->getType()->getMetaCategory())
	{
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
	if(0 != member)
	{
		paf::Type* memberType = member->getType();
		switch(memberType->getMetaCategory())
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
	else if (name[0] == '_')
	{
		switch (name[1])
		{
		case '\0':
			if ((variant->getType()->isPrimitive() || variant->getType()->isEnumeration()) && (variant->getCategory() == paf::Variant::vt_small_value))
			{
				return SetPrimitiveOrEnum(L, variant);
			}
			break;
		case 'c':
			if (strcmp(&name[2], "ount_") == 0)//_count_
			{
				return SetArraySize(L, variant);
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

enum SubscriptCategory
{
	sc_error,
	sc_integer,
	sc_string,
};

SubscriptCategory Variant_ParseSubscript(size_t& num, const char*& str, lua_State *L)
{
	int index = 2;
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TSTRING: {
		str = lua_tostring(L, index);
		return sc_string;
		break; }
	case LUA_TBOOLEAN: {
		num = lua_toboolean(L, index) ? 1 : 0;
		return sc_integer;
		break; }
	case LUA_TNUMBER: {
		num = lua_tointeger(L, index);
		return sc_integer;
		break; }
	case LUA_TUSERDATA:{
		paf::Variant* variant = (paf::Variant*)luaL_checkudata(L, index, variant_metatable_name);
		if (variant)
		{
			if (variant->isNull())
			{
				return sc_error;
			}
			else if (variant->isValue())
			{
				paf::Type* type = variant->getType();
				if (type->isPrimitive()) 
				{
					paf::PrimitiveType* primitiveType = static_cast<paf::PrimitiveType*>(type);
					primitiveType->castTo(&num, RuntimeTypeOf<size_t>::RuntimeType::GetSingleton(), variant->getRawPointer());
					return sc_integer;
				}
				else if (type->isEnumeration())
				{
					paf::EnumType* enumType = static_cast<paf::EnumType*>(type);
					enumType->castToPrimitive(&num, RuntimeTypeOf<size_t>::RuntimeType::GetSingleton(), variant->getRawPointer());
					return sc_integer;
				}
				else
				{
					paf::ClassType* classType = static_cast<paf::ClassType*>(type);
					if (paf::string_t::GetType() == classType)
					{
						str = static_cast<paf::string_t*>(variant->getRawPointer())->c_str();
						return sc_string;
					}
					else
					{
						size_t offset;
						if (classType->getClassOffset(offset, paf::StringBase::GetType()))
						{
							str = reinterpret_cast<paf::StringBase*>((size_t)variant->getRawPointer() + offset)->toString();
							return sc_string;
						}
					}
				}
			}
		}
		break; }
	}
	return sc_error;
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
			Variant_Error(L, buf, errorCode);
		}
		else if (sc_string == sc)
		{
			sprintf_s(buf, "__index[\"%s\"]", str);
			Variant_Error(L, buf, errorCode);
		}
		else
		{
			Variant_Error(L, "__index", errorCode);
		}
		return 0;
	}
	return 1;
}

paf::ErrorCode Variant_NewIndex_Subscript(lua_State *L, paf::Variant* variant, size_t index)
{
	paf::Variant item;
	paf::ErrorCode errorCode = variant->subscript(item, index);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	paf::Variant value;
	paf::Variant* arg = LuaToVariant(&value, L, 3);

	paf::Type* dstType = item.getType();
	if (dstType->isPrimitive())
	{
		if (arg->castToPrimitive(static_cast<paf::PrimitiveType*>(dstType), item.getRawPointer()))
		{
			return paf::ErrorCode::s_ok;
		}
	}
	else if (dstType->isEnumeration())
	{
		if (arg->castToEnum(static_cast<paf::EnumType*>(dstType), item.getRawPointer()))
		{
			return paf::ErrorCode::s_ok;
		}
	}
	else
	{
		paf::ClassType* classType = static_cast<paf::ClassType*>(dstType);
		size_t offset;
		if (classType->getClassOffset(offset, paf::StringBase::GetType()))
		{
			paf::StringBase* dstStr = (paf::StringBase*)((size_t)variant->getRawPointer() + offset);
			paf::string_t srcStr;
			if (arg->castToString(srcStr))
			{
				dstStr->fromString(srcStr);
				return paf::ErrorCode::s_ok;
			}
		}
		else if (classType->getClassOffset(offset, paf::BufferBase::GetType()))
		{
			paf::BufferBase* dstBuf = (paf::BufferBase*)((size_t)variant->getRawPointer() + offset);
			paf::buffer_t srcBuf;
			if (arg->castToBuffer(srcBuf))
			{
				dstBuf->fromBuffer(srcBuf);
				return paf::ErrorCode::s_ok;
			}
		}
		else if (dstType == arg->getType())
		{
			if (classType->copyAssign(item.getRawPointer(), arg->getRawPointer()))
			{
				return paf::ErrorCode::s_ok;
			}
		}
	}
	return paf::ErrorCode::e_invalid_type;
}


int Variant_NewIndex(lua_State *L) 
{
	paf::Variant* variant = (paf::Variant*)lua_touserdata(L, 1);
	if(variant->isNull())
	{
		Variant_Error(L, "__newindex", paf::ErrorCode::e_is_null);
		return 0;
	}
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
			Variant_Error(L, buf, errorCode);
		}
		else if (sc_string == sc)
		{
			sprintf_s(buf, "__newindex[\"%s\"]", str);
			Variant_Error(L, buf, errorCode);
		}
		else
		{
			Variant_Error(L, "__newindex", errorCode);
		}
		return 0;
	}
	return 1;
}

struct luaL_Reg g_variant_reg [] = 
{
	{ "__gc", Variant_GC },
	{ "__index", Variant_Index },
	{ "__newindex", Variant_NewIndex },
	{ "__call", Variant_Call },
	{ "__len", Variant_Len },
	{ "__lt", Variant_Less },
	{ "__eq", Variant_Equal },
	{ "__le", Variant_LessEqual },
	{NULL, NULL}
};

int InstanceArrayProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, "__index for instanceArrayProperty ", paf::ErrorCode::e_is_null);
		return 0;
	}
	size_t num;
	const char* str;
	SubscriptCategory sc = Variant_ParseSubscript(num, str, L);
	if (sc_integer == sc)
	{
		errorCode = GetInstanceArrayProperty(L, instance->object, instance->property, num);
	}
	else if (sc_string == sc)
	{
		if (strcmp("_count_", str) == 0)
		{
			errorCode = GetInstanceArrayPropertySize(L, instance->object, instance->property);
		}
		else
		{
			errorCode = paf::ErrorCode::e_member_not_found;
		}
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__index for instanceArrayProperty ", errorCode);
		return 0;
	}
	return 1;
}

int InstanceArrayProperty_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, "__newindex for instanceArrayProperty ", paf::ErrorCode::e_is_null);
		return 0;
	}
	size_t num;
	const char* str;
	SubscriptCategory sc = Variant_ParseSubscript(num, str, L);
	if (sc_integer == sc)
	{
		errorCode = SetInstanceArrayProperty(L, instance->object, instance->property, num);
	}
	//else if (sc_string == sc)
	//{
	//	if (strcmp("_count_", str) == 0)
	//	{
	//		errorCode = SetInstanceArrayPropertySize(L, instance->object, instance->property);
	//	}
	//	else
	//	{
	//		errorCode = paf::ErrorCode::e_member_not_found;
	//	}
	//}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__newindex for instanceArrayProperty ", errorCode);
		return 0;
	}
	return 1;
}


int InstanceArrayProperty_Len(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, "# for instanceArrayProperty", paf::ErrorCode::e_is_null);
		return 0;
	}
	paf::InstanceProperty* property = instance->property;
	assert(property->get_propertyCategory()== paf::PropertyCategory::array_property);
	size_t size;
	errorCode = (*property->m_arraySize)(property, instance->object, size);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		lua_pushinteger(L, size);
		return 1;
	}
	else
	{
		Variant_Error(L, "# for instanceArrayProperty", errorCode);
		return 0;
	}
}

struct luaL_Reg g_instanceArrayPropertyInstance_reg[] =
{
	{ "__index", InstanceArrayProperty_Index },
	{ "__newindex", InstanceArrayProperty_NewIndex },
	{ "__len", InstanceArrayProperty_Len },
	{ NULL, NULL }
};


int StaticArrayProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	size_t num;
	const char* str;
	SubscriptCategory sc = Variant_ParseSubscript(num, str, L);
	if (sc_integer == sc)
	{
		errorCode = GetStaticArrayProperty(L, instance->property, num);
	}
	else if (sc_string == sc)
	{
		if (strcmp("_count_", str) == 0)
		{
			errorCode = GetStaticArrayPropertySize(L, instance->property);
		}
		else
		{
			errorCode = paf::ErrorCode::e_member_not_found;
		}
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__index for staticArrayProperty", errorCode);
		return 0;
	}
	return 1;
}

int StaticArrayProperty_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	size_t num;
	const char* str;
	SubscriptCategory sc = Variant_ParseSubscript(num, str, L);
	if (sc_integer == sc)
	{
		errorCode = SetStaticArrayProperty(L, instance->property, num);
	}
	//else if (sc_string == sc)
	//{
	//	if (strcmp("_count_", str) == 0)
	//	{
	//		errorCode = SetStaticArrayPropertySize(L, instance->property);
	//	}
	//	else
	//	{
	//		errorCode = paf::ErrorCode::e_member_not_found;
	//	}
	//}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__newindex for staticArrayProperty", errorCode);
		return 0;
	}
	return 1;
}

int StaticArrayProperty_Len(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	paf::StaticProperty* property = instance->property;
	assert(property->get_propertyCategory() == paf::PropertyCategory::array_property);
	size_t size;
	errorCode = (*property->m_arraySize)(property, size);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		lua_pushinteger(L, size);
		return 1;
	}
	else
	{
		Variant_Error(L, "# for staticArrayProperty", errorCode);
		return 0;
	}
}

struct luaL_Reg g_staticArrayPropertyInstance_reg[] =
{
	{ "__index", StaticArrayProperty_Index },
	{ "__newindex", StaticArrayProperty_NewIndex },
	{ "__len", StaticArrayProperty_Len },
	{ NULL, NULL }
};


int InstanceMapProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, "__index for instanceMapProperty", paf::ErrorCode::e_is_null);
		return 0;
	}
	paf::Variant variant;
	paf::Variant* var = LuaToVariant(&variant, L, 2);

	paf::Iterator* iterator;
	if (!var->castToRawPointer(paf::Iterator::GetType(), (void**)&iterator))
	{
		Variant_Error(L, "__index for instanceMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}

	errorCode = GetInstanceMapProperty(L, instance->object, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__index for instanceMapProperty", errorCode);
		return 0;
	}
	return 1;
}

int InstanceMapProperty_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	if (0 == instance->object)
	{
		Variant_Error(L, "__newindex for instanceMapProperty", paf::ErrorCode::e_is_null);
		return 0;
	}
	paf::Variant variant;
	paf::Variant* var = LuaToVariant(&variant, L, 2);

	paf::Iterator* iterator;
	if (!var->castToRawPointer(paf::Iterator::GetType(), (void**)&iterator))
	{
		Variant_Error(L, "__index for instanceMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}
	errorCode = SetInstanceMapProperty(L, instance->object, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__newindex for instanceMapProperty", errorCode);
		return 0;
	}
	return 1;
}

struct luaL_Reg g_instanceMapPropertyInstance_reg[] =
{
	{ "__index", InstanceMapProperty_Index },
	{ "__newindex", InstanceMapProperty_NewIndex },
	//{ "__len", InstanceMapProperty_Len },
	{ NULL, NULL }
};


int StaticMapProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	paf::Variant variant;
	paf::Variant* var = LuaToVariant(&variant, L, 2);
	paf::Iterator* iterator;
	if (!var->castToRawPointer(paf::Iterator::GetType(), (void**)&iterator))
	{
		Variant_Error(L, "__index for staticMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}
	errorCode = GetStaticMapProperty(L, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__index for staticMapProperty", errorCode);
		return 0;
	}
	return 1;
}

int StaticMapProperty_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	paf::Variant variant;
	paf::Variant* var = LuaToVariant(&variant, L, 2);
	paf::Iterator* iterator;
	if (!var->castToRawPointer(paf::Iterator::GetType(), (void**)&iterator))
	{
		Variant_Error(L, "__newindex for staticMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}
	errorCode = SetStaticMapProperty(L, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		Variant_Error(L, "__newindex for staticMapProperty", errorCode);
		return 0;
	}
	return 1;
}

struct luaL_Reg g_staticMapPropertyInstance_reg[] =
{
	{ "__index", StaticMapProperty_Index, },
	{ "__newindex", StaticMapProperty_NewIndex, },
	{ NULL, NULL, }
};

END_PAFLUA
