#include "Utility.h"
#include "../3rd/lua/src/lua.hpp"
#include "../pafcore/Utility.h"
#include "../pafcore/Utility.mh"
#include "../pafcore/String.h"
#include "../pafcore/String.mh"
#include "../pafcore/Type.h"
#include "../pafcore/PrimitiveType.h"
#include "../pafcore/EnumType.h"
#include "../pafcore/Variant.h"
#include <new.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <Windows.h>

BEGIN_PAFLUA

extern const char* variant_metatable_name;

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

void stackDump(lua_State *L)
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
			if (lua_isinteger(L, i))
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

void RaiseLuaError(lua_State *L, const char* name, paf::ErrorCode errorCode)
{
	luaL_error(L, "idlcpp error: %s, Error: %d, %s\n", name, errorCode, paf::ErrorCodeToString(errorCode));
}

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
	case LUA_TUSERDATA: {
		paf::Variant* variant = (paf::Variant*)luaL_checkudata(L, index, variant_metatable_name);
		if (variant)
		{
			if (variant->castToValue(num))
			{
				return sc_integer;
			}
			paf::string_t s;
			if (variant->castToValue(s))
			{
				str = s.c_str();
				return sc_string;
			}
		}
		else
		{
			str = luaL_tolstring(L, index, nullptr);
			if (str)
			{
				return sc_string;
			}
		}
		break; }
	}
	return sc_error;
}

bool Variant_ParseIntegerSubscript(size_t& num, lua_State *L)
{
	int index = 2;
	int type = lua_type(L, index);
	switch (type)
	{
	case LUA_TBOOLEAN: {
		num = lua_toboolean(L, index) ? 1 : 0;
		return true;
		break; }
	case LUA_TNUMBER: {
		num = lua_tointeger(L, index);
		return true;
		break; }
	case LUA_TUSERDATA: {
		paf::Variant* variant = (paf::Variant*)luaL_checkudata(L, index, variant_metatable_name);
		if (variant)
		{
			if (variant->castToPrimitive(num))
			{
				return true;
			}
		}
		break; }
	}
	return false;
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
		value->newValue<paf::String>(s);
	}
	break;
	case LUA_TBOOLEAN:
	{
		bool b = lua_toboolean(L, index) != 0;
		value->assignValue<bool>(b);
	}
	break;
	case LUA_TNUMBER:
		if (lua_isinteger(L, index))
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
		if (variant)
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
	if (variant->isValue() || variant->isReference())
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
			enumType->cast(RuntimeTypeOf<lua_Integer>::RuntimeType::GetSingleton(), &value, variant->getRawPointer());
			lua_pushinteger(L, value);
			return;
		}
	}
	void* p = lua_newuserdata(L, sizeof(paf::Variant));
	paf::Variant* res = new(p)paf::Variant(std::move(*variant));
	luaL_getmetatable(L, variant_metatable_name);
	lua_setmetatable(L, -2);
}


END_PAFLUA
