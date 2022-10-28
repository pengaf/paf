#pragma once
#include "Utility.h"
#include "../3rd/lua/src/lua.hpp"
#include "../pafcore/Field.h"
BEGIN_PAFLUA

extern const char* instanceArrayField_metatable_name;
extern const char* staticArrayField_metatable_name;

extern struct luaL_Reg g_instanceArrayFieldInstance_reg[];
extern struct luaL_Reg g_staticArrayFieldInstance_reg[];

paf::ErrorCode MakeStaticArrayField(lua_State *L, paf::StaticField* field);
paf::ErrorCode MakeInstanceArrayField(lua_State *L, paf::Variant* that, paf::InstanceField* field);

END_PAFLUA


