#include "ArrayPropertyWrapper.h"
#include "../pafcore/Property.h"
#include "../pafcore/Variant.h"

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

int InstanceArrayProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = GetInstanceArrayProperty(L, instance->object, instance->property, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__index for instanceArrayProperty ", errorCode);
		return 0;
	}
	return 1;
}

int InstanceArrayProperty_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = SetInstanceArrayProperty(L, instance->object, instance->property, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__newindex for instanceArrayProperty ", errorCode);
		return 0;
	}
	return 1;
}


int InstanceArrayProperty_Len(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstancePropertyInstance* instance = (InstancePropertyInstance*)lua_touserdata(L, 1);
	paf::InstanceProperty* property = instance->property;
	assert(property->get_propertyCategory() == paf::PropertyCategory::array_property);
	size_t size;
	errorCode = (*property->m_arraySize)(property, instance->object, size);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		lua_pushinteger(L, size);
		return 1;
	}
	else
	{
		RaiseLuaError(L, "# for instanceArrayProperty", errorCode);
		return 0;
	}
}


int StaticArrayProperty_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = GetStaticArrayProperty(L, instance->property, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__index for staticArrayProperty", errorCode);
		return 0;
	}
	return 1;
}

int StaticArrayProperty_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticPropertyInstance* instance = (StaticPropertyInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = SetStaticArrayProperty(L, instance->property, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__newindex for staticArrayProperty", errorCode);
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
		RaiseLuaError(L, "# for staticArrayProperty", errorCode);
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

struct luaL_Reg g_staticArrayPropertyInstance_reg[] =
{
	{ "__index", StaticArrayProperty_Index },
	{ "__newindex", StaticArrayProperty_NewIndex },
	{ "__len", StaticArrayProperty_Len },
	{ NULL, NULL }
};

const char* instanceArrayProperty_metatable_name = "paf.InstanceArrayProperty";
const char* staticArrayProperty_metatable_name = "paf.StaticArrayProperty";

END_PAFLUA
