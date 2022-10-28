#pragma once
#include "Utility.h"
#include "../3rd/lua/src/lua.hpp"
#include "../pafcore/Property.h"

BEGIN_PAFLUA

extern const char* instanceMapProperty_metatable_name;
extern const char* staticMapProperty_metatable_name;

extern struct luaL_Reg g_instanceMapPropertyInstance_reg[];
extern struct luaL_Reg g_staticMapPropertyInstance_reg[];

paf::ErrorCode MakeStaticMapProperty(lua_State *L, paf::StaticProperty* property);
paf::ErrorCode MakeInstanceMapProperty(lua_State *L, paf::Variant* that, paf::InstanceProperty* property);

END_PAFLUA


