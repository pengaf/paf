#include "LuaSubclassInvoker.h"
#include "../3rd/lua/src/lua.hpp"
#include "../pafcore/Variant.h"
#include <Windows.h>

BEGIN_PAFLUA

extern const char* variant_metatable_name;
void stackDump (lua_State *L);
paf::Variant* LuaToVariant(paf::Variant* value, lua_State *L, int index);
void VariantToLua(lua_State *L, paf::Variant* variant);

//paf::Variant* VariantToLua2(lua_State *L, paf::Variant* variant)
//{
//	void* p = lua_newuserdata(L, sizeof(paf::Variant));
//	paf::Variant* res = new(p)paf::Variant;
//	res->move(*variant);
//	luaL_getmetatable(L, variant_metatable_name);
//	lua_setmetatable(L, -2);
//	return res;
//}

LuaSubclassInvoker::LuaSubclassInvoker(lua_State* luaState)
{
	m_luaState = luaState;
	lua_rawsetp(luaState, LUA_REGISTRYINDEX, this);
}

LuaSubclassInvoker::~LuaSubclassInvoker()
{
	lua_pushnil(m_luaState);
	lua_rawsetp(m_luaState, LUA_REGISTRYINDEX, this);
}

paf::ErrorCode LuaSubclassInvoker::invoke(const char* name, paf::Variant* result, paf::Variant* self, paf::Variant* args, size_t numArgs)
{
#ifdef _DEBUG
	int top = lua_gettop(m_luaState);
	//stackDump(m_luaState);
#endif
	
	lua_rawgetp(m_luaState, LUA_REGISTRYINDEX, this);

	//stackDump(m_luaState);

	lua_getfield(m_luaState, -1, name);

	//stackDump(m_luaState);

	if (!(lua_isfunction(m_luaState, -1) || lua_iscfunction(m_luaState, -1)))
	{
		lua_pop(m_luaState, 2);

		return paf::ErrorCode::e_script_dose_not_override;
	}

	lua_insert(m_luaState, -2);

	//stackDump(m_luaState);

	for(size_t i = 0; i < numArgs; ++i)
	{
		//luaArgs[i] = VariantToLua2(m_luaState, &args[i]);
		VariantToLua(m_luaState, &args[i]);
	}

	//stackDump(m_luaState);

	int error = lua_pcall(m_luaState, int(numArgs + 1), 1, 0);

	//stackDump(m_luaState);

	if (error)
	{
		const char* str = lua_tostring(m_luaState, -1);
//#ifdef _DEBUG
		OutputDebugStringA(str);
		OutputDebugStringA("\n");
//#endif
		lua_pop(m_luaState, 1);

		//stackDump(m_luaState);

	}
	else
	{

		paf::Variant* value = LuaToVariant(result, m_luaState, -1);
		if(value != result)
		{
			result->assign(std::move(*value));
		}

		//stackDump(m_luaState);

		//for(size_t i = 0; i < numArgs; ++i)
		//{
		//	args[i].move(*luaArgs[i]);
		//}
		lua_pop(m_luaState, 1);
	}

#ifdef _DEBUG
	PAF_ASSERT(lua_gettop(m_luaState) == top);
	//stackDump(m_luaState);
#endif

	return (0 == error) ? paf::ErrorCode::s_ok : paf::ErrorCode::e_script_dose_not_override;
}

END_PAFLUA
