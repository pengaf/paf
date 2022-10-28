#include "CollectionPropertyWrapper.h"
#include "../pafcore/Property.h"
#include "../pafcore/Variant.h"
#include "../pafcore/ClassType.h"

BEGIN_PAFLUA

struct InstancePropertyInstance
{
	paf::InstanceProperty* property;
	paf::Variant* object;
};

struct StaticPropertyInstance
{
	paf::StaticProperty* property;
};

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

int InstanceMapProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	paf::Variant variant;
	paf::Variant* var = LuaToVariant(&variant, L, 2);

	paf::Iterator* iterator;
	if (!var->castToRawPointer(paf::Iterator::GetType(), (void**)&iterator))
	{
		RaiseLuaError(L, "__index for instanceMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}

	errorCode = GetInstanceMapProperty(L, instance->object, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__index for instanceMapProperty", errorCode);
		return 0;
	}
	return 1;
}

int InstanceMapProperty_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	paf::Variant variant;
	paf::Variant* var = LuaToVariant(&variant, L, 2);

	paf::Iterator* iterator;
	if (!var->castToRawPointer(paf::Iterator::GetType(), (void**)&iterator))
	{
		RaiseLuaError(L, "__index for instanceMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}
	errorCode = SetInstanceMapProperty(L, instance->object, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__newindex for instanceMapProperty", errorCode);
		return 0;
	}
	return 1;
}


int StaticMapProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	paf::Variant variant;
	paf::Variant* var = LuaToVariant(&variant, L, 2);
	paf::Iterator* iterator;
	if (!var->castToRawPointer(paf::Iterator::GetType(), (void**)&iterator))
	{
		RaiseLuaError(L, "__index for staticMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}
	errorCode = GetStaticMapProperty(L, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__index for staticMapProperty", errorCode);
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
		RaiseLuaError(L, "__newindex for staticMapProperty", paf::ErrorCode::e_invalid_type);
		return 0;
	}
	errorCode = SetStaticMapProperty(L, instance->property, iterator);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__newindex for staticMapProperty", errorCode);
		return 0;
	}
	return 1;
}

struct luaL_Reg g_instanceMapPropertyInstance_reg[] =
{
	{ "__index", InstanceMapProperty_Index },
	{ "__newindex", InstanceMapProperty_NewIndex },
	{ NULL, NULL }
};

struct luaL_Reg g_staticMapPropertyInstance_reg[] =
{
	{ "__index", StaticMapProperty_Index, },
	{ "__newindex", StaticMapProperty_NewIndex, },
	{ NULL, NULL, }
};

const char* instanceMapProperty_metatable_name = "paf.InstanceMapProperty";
const char* staticMapProperty_metatable_name = "paf.StaticMapProperty";

END_PAFLUA
