#pragma once

#include "../pafcore/Utility.h"
#include "../3rd/lua/src/lua.hpp"

#if defined PAFLUA_EXPORTS
	#define PAFLUA_EXPORT __declspec(dllexport)
#else
	#define PAFLUA_EXPORT __declspec(dllimport)
#endif

#define BEGIN_PAFLUA namespace paflua {
#define END_PAFLUA }


BEGIN_PAFLUA

void RaiseLuaError(lua_State *L, const char* name, paf::ErrorCode errorCode);
bool LuaToInt(int& value, lua_State *L, int index);
paf::Variant* LuaToVariant(paf::Variant* value, lua_State *L, int index);
void VariantToLua(lua_State *L, paf::Variant* variant);
bool Variant_ParseIntegerSubscript(size_t& num, lua_State *L);


enum SubscriptCategory
{
	sc_error,
	sc_integer,
	sc_string,
};


SubscriptCategory Variant_ParseSubscript(size_t& num, const char*& str, lua_State *L);

END_PAFLUA
