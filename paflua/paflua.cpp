#include "paflua.h"
#include "../3rd/lua/lua.hpp"
#include "VariantWrapper.h"
#include "ArrayFieldWrapper.h"
#include "ArrayPropertyWrapper.h"
#include "CollectionPropertyWrapper.h"
#include "VariantWrapper.h"
#include "../pafcore/Variant.h"
#include "../pafcore/NameSpace.h"
#include "../pafcore/NameSpace.mh"
#include <new.h>
#include <Windows.h>

void init_paf(lua_State *L)
{
	luaL_newmetatable(L, paflua::instanceArrayProperty_metatable_name);
	luaL_setfuncs(L, paflua::g_instanceArrayPropertyInstance_reg, 0);

	luaL_newmetatable(L, paflua::staticArrayProperty_metatable_name);
	luaL_setfuncs(L, paflua::g_staticArrayPropertyInstance_reg, 0);

	luaL_newmetatable(L, paflua::instanceMapProperty_metatable_name);
	luaL_setfuncs(L, paflua::g_instanceMapPropertyInstance_reg, 0);

	luaL_newmetatable(L, paflua::staticMapProperty_metatable_name);
	luaL_setfuncs(L, paflua::g_staticMapPropertyInstance_reg, 0);

	luaL_newmetatable(L, paflua::instanceArrayField_metatable_name);
	luaL_setfuncs(L, paflua::g_instanceArrayFieldInstance_reg, 0);

	luaL_newmetatable(L, paflua::staticArrayField_metatable_name);
	luaL_setfuncs(L, paflua::g_staticArrayFieldInstance_reg, 0);

	luaL_newmetatable(L, paflua::variant_metatable_name);
	luaL_setfuncs(L, paflua::g_variant_reg, 0);

	void* p = lua_newuserdata(L, sizeof(paf::Variant));
	paf::Variant* variant = new(p)paf::Variant;
	variant->assignRawPtr(RuntimeTypeOf<paf::NameSpace>::RuntimeType::GetSingleton(), paf::NameSpace::GetGlobalNameSpace());
	luaL_getmetatable(L, paflua::variant_metatable_name);
	lua_setmetatable(L, -2);
	lua_setglobal(L, "idlcpp");
}

int luaopen_paflua(lua_State *L)
{
	init_paf(L);
	return 1;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (DLL_PROCESS_ATTACH == dwReason)
	{
		OutputDebugStringA("paflua Process Attach\n");
	}
	else if (DLL_PROCESS_DETACH == dwReason)
	{
		OutputDebugStringA("paflua Process Detach\n");
	}
	return TRUE;
}
