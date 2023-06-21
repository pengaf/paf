#include "ArrayFieldWrapper.h"
#include "../pafcore/Field.h"
#include "../pafcore/Variant.h"
#include "../pafcore/Reflection.h"

BEGIN_PAFLUA

struct InstanceFieldInstance
{
	paf::InstanceField* field;
	paf::Variant* object;
};

struct StaticFieldInstance
{
	paf::StaticField* field;
};

paf::ErrorCode GetInstanceArrayField(lua_State *L, paf::Variant* that, paf::InstanceField* field, size_t index)
{
	return paf::ErrorCode::s_ok;
	//paf::Variant value;
	//paf::ErrorCode errorCode = paf::Reflection::GetInstanceArrayField(value, that, field, index);
	//if (paf::ErrorCode::s_ok == errorCode)
	//{
	//	VariantToLua(L, &value);
	//}
	//return errorCode;
}

paf::ErrorCode SetInstanceArrayField(lua_State *L, paf::Variant* that, paf::InstanceField* field, size_t index)
{
	return paf::ErrorCode::s_ok;
	//paf::Variant value;
	//paf::Variant* arg = LuaToVariant(&value, L, 3);
	//paf::ErrorCode errorCode = paf::Reflection::SetInstanceArrayField(that, field, index, arg);
	//return errorCode;
}

paf::ErrorCode GetInstanceArrayFieldSize(lua_State *L, paf::Variant* that, paf::InstanceField* field)
{
	size_t size = field->get_arraySize();
	lua_pushinteger(L, size);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode MakeInstanceArrayField(lua_State *L, paf::Variant* that, paf::InstanceField* field)
{
	assert(field->get_arraySize() > 0);

	void* p = lua_newuserdata(L, sizeof(InstanceFieldInstance));
	((InstanceFieldInstance*)p)->field = field;
	((InstanceFieldInstance*)p)->object = that;
	luaL_getmetatable(L, instanceArrayField_metatable_name);
	lua_setmetatable(L, -2);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetStaticArrayField(lua_State *L, paf::StaticField* field, size_t index)
{
	return paf::ErrorCode::s_ok;
	//paf::Variant value;
	//paf::ErrorCode errorCode = paf::Reflection::GetStaticArrayField(value, field, index);
	//if (paf::ErrorCode::s_ok == errorCode)
	//{
	//	VariantToLua(L, &value);
	//}
	//return errorCode;
}

paf::ErrorCode SetStaticArrayField(lua_State *L, paf::StaticField* field, size_t index)
{
	return paf::ErrorCode::s_ok;
	//paf::Variant value;
	//paf::Variant* arg = LuaToVariant(&value, L, 3);
	//paf::ErrorCode errorCode = paf::Reflection::SetStaticArrayField(field, index, arg);
	//return errorCode;
}

paf::ErrorCode GetStaticArrayFieldSize(lua_State *L, paf::StaticField* field)
{
	size_t size = field->get_arraySize();
	lua_pushinteger(L, size);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode MakeStaticArrayField(lua_State *L, paf::StaticField* field)
{
	assert(field->get_arraySize() > 0);
	void* p = lua_newuserdata(L, sizeof(StaticFieldInstance));
	((StaticFieldInstance*)p)->field = field;
	luaL_getmetatable(L, staticArrayField_metatable_name);
	lua_setmetatable(L, -2);
	return paf::ErrorCode::s_ok;
}

int InstanceArrayField_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstanceFieldInstance* instance = (InstanceFieldInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = GetInstanceArrayField(L, instance->object, instance->field, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__index for instanceArrayField ", errorCode);
		return 0;
	}
	return 1;
}

int InstanceArrayField_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstanceFieldInstance* instance = (InstanceFieldInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = SetInstanceArrayField(L, instance->object, instance->field, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__newindex for instanceArrayField ", errorCode);
		return 0;
	}
	return 1;
}


int InstanceArrayField_Len(lua_State *L)
{
	paf::ErrorCode errorCode;
	InstanceFieldInstance* instance = (InstanceFieldInstance*)lua_touserdata(L, 1);
	paf::InstanceField* field = instance->field;
	uint32_t size = field->get_arraySize();
	lua_pushinteger(L, size);
	return 1;
}


int StaticArrayField_Index(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticFieldInstance* instance = (StaticFieldInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = GetStaticArrayField(L, instance->field, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__index for staticArrayField", errorCode);
		return 0;
	}
	return 1;
}

int StaticArrayField_NewIndex(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticFieldInstance* instance = (StaticFieldInstance*)lua_touserdata(L, 1);
	size_t num;
	if (Variant_ParseIntegerSubscript(num, L))
	{
		errorCode = SetStaticArrayField(L, instance->field, num);
	}
	else
	{
		errorCode = paf::ErrorCode::e_invalid_subscript_type;
	}
	if (paf::ErrorCode::s_ok != errorCode)
	{
		RaiseLuaError(L, "__newindex for staticArrayField", errorCode);
		return 0;
	}
	return 1;
}

int StaticArrayField_Len(lua_State *L)
{
	paf::ErrorCode errorCode;
	StaticFieldInstance* instance = (StaticFieldInstance*)lua_touserdata(L, 1);
	paf::StaticField* field = instance->field;
	uint32_t size = field->get_arraySize();
	lua_pushinteger(L, size);
	return 1;
}

struct luaL_Reg g_instanceArrayFieldInstance_reg[] =
{
	{ "__index", InstanceArrayField_Index },
	{ "__newindex", InstanceArrayField_NewIndex },
	{ "__len", InstanceArrayField_Len },
	{ NULL, NULL }
};

struct luaL_Reg g_staticArrayFieldInstance_reg[] =
{
	{ "__index", StaticArrayField_Index },
	{ "__newindex", StaticArrayField_NewIndex },
	{ "__len", StaticArrayField_Len },
	{ NULL, NULL }
};

const char* instanceArrayField_metatable_name = "paf.InstanceArrayField";
const char* staticArrayField_metatable_name = "paf.StaticArrayField";

END_PAFLUA
