#pragma once
#include "Utility.h"
#include "../3rd/lua/src/lua.hpp"
#include "../pafcore/Property.h"

BEGIN_PAFLUA

extern const char* instanceArrayProperty_metatable_name;
extern const char* staticArrayProperty_metatable_name;

extern struct luaL_Reg g_instanceArrayPropertyInstance_reg[];
extern struct luaL_Reg g_staticArrayPropertyInstance_reg[];

paf::ErrorCode MakeStaticArrayProperty(lua_State *L, paf::StaticProperty* property);
paf::ErrorCode MakeInstanceArrayProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property);

END_PAFLUA


