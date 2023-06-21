#pragma once

#include "Utility.h"
#include "../3rd/lua/lua.hpp"

BEGIN_PAFLUA

struct LuaSubclassInvoker : public paf::SubclassInvoker
{
public:
	LuaSubclassInvoker(lua_State* luaState);
	~LuaSubclassInvoker();
public:
	virtual paf::ErrorCode invoke(const char* name, paf::Variant* self, paf::Variant* results, size_t numResults, paf::Variant* args, size_t numArgs);
public:
	lua_State* m_luaState;
};

END_PAFLUA
