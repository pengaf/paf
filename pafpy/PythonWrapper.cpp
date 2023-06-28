#include "Utility.h"

#include "../pafcore/Method.h"
#include "../pafcore/NameSpace.h"
#include "../pafcore/ClassType.h"
#include "../pafcore/EnumType.h"
#include "../pafcore/Field.h"
#include "../pafcore/Property.h"
#include "../pafcore/EnumType.h"
#include "../pafcore/PrimitiveType.h"
#include "../pafcore/String.h"

#include "../pafcore/Utility.mh"
#include "../pafcore/Metadata.mh"
#include "../pafcore/NameSpace.mh"
#include "../pafcore/EnumType.mh"
#include "../pafcore/Type.mh"
#include "../pafcore/String.mh"
#include "../pafcore/Variant.h"

#include "PythonWrapper.h"
#include "PythonSubclassInvoker.h"

BEGIN_PAFPY

const size_t max_param_count = 20;
const size_t max_result_count = 10;

struct VariantWrapper
{
	PyObject_HEAD
	char m_var[sizeof(paf::Variant)];
};

static PyTypeObject s_VariantWrapper_Type = 
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"PafPython.Object",
	sizeof(VariantWrapper),
};

struct InstanceArrayPropertyWrapper
{
	PyObject_HEAD
	paf::InstanceProperty* m_property;
	VariantWrapper* m_self;
};

static PyTypeObject s_InstanceArrayPropertyWrapper_Type =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"PafPython.InstanceArrayProperty",
	sizeof(InstanceArrayPropertyWrapper),
};

struct StaticArrayPropertyWrapper
{
	PyObject_HEAD
	paf::StaticProperty* m_property;
};

static PyTypeObject s_StaticArrayPropertyWrapper_Type =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"PafPython.StaticArrayProperty",
	sizeof(StaticArrayPropertyWrapper),
};

struct InstanceMethodWrapper
{
	PyObject_HEAD
	paf::InstanceMethod* m_method;
	VariantWrapper* m_self;
};

static PyTypeObject s_InstanceMethodWrapper_Type =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"PafPython.InstanceMethod",
	sizeof(InstanceMethodWrapper),
};

struct FunctionInvokerWrapper
{
	PyObject_HEAD
	paf::InvokeMethod m_invoker;
};

static PyTypeObject s_FunctionInvokerWrapper_Type = 
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"PafPython.FunctionInvoker",
	sizeof(FunctionInvokerWrapper),
};

struct ProxyCreatorWrapper
{
	PyObject_HEAD
	paf::ClassType* m_classType;
};

static PyTypeObject s_ProxyCreatorWrapper_Type =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"PafPython.ProxyCreator",
	sizeof(ProxyCreatorWrapper),
};

PyObject* VariantWrapper_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	VariantWrapper* self;
	self = (VariantWrapper*)type->tp_alloc(type, 0);
	if (self != NULL)
	{
		void* var = self->m_var;
		paf::Variant* variant = new(var)paf::Variant;
	}
	return (PyObject*)self;
}

void VariantWrapper_dealloc(VariantWrapper* self)
{
	paf::Variant* variant = (paf::Variant*)self->m_var;
	variant->~Variant();
	Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* FunctionInvokerWrapper_new(PyTypeObject* type, PyObject *args, PyObject *kwds)
{
	FunctionInvokerWrapper* self;
	self = (FunctionInvokerWrapper*)type->tp_alloc(type, 0);
	self->m_invoker = 0;
	return (PyObject*)self;
}

void InstanceArrayPropertyWrapper_dealloc(InstanceArrayPropertyWrapper* self)
{
	Py_XDECREF(self->m_self);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* InstanceArrayPropertyWrapper_new(PyTypeObject* type, PyObject *args, PyObject *kwds)
{
	InstanceArrayPropertyWrapper* self;
	self = (InstanceArrayPropertyWrapper*)type->tp_alloc(type, 0);
	self->m_property = 0;
	self->m_self = 0;
	return (PyObject*)self;
}

PyObject* StaticArrayPropertyWrapper_new(PyTypeObject* type, PyObject *args, PyObject *kwds)
{
	StaticArrayPropertyWrapper* self;
	self = (StaticArrayPropertyWrapper*)type->tp_alloc(type, 0);
	self->m_property = 0;
	return (PyObject*)self;
}

void InstanceMethodWrapper_dealloc(InstanceMethodWrapper* self)
{
	Py_XDECREF(self->m_self);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* InstanceMethodWrapper_new(PyTypeObject* type, PyObject *args, PyObject *kwds)
{
	InstanceMethodWrapper* self;
	self = (InstanceMethodWrapper*)type->tp_alloc(type, 0);
	self->m_method = 0;
	self->m_self = 0;
	return (PyObject*)self;
}

PyObject* ProxyCreatorWrapper_new(PyTypeObject* type, PyObject *args, PyObject *kwds)
{
	ProxyCreatorWrapper* self;
	self = (ProxyCreatorWrapper*)type->tp_alloc(type, 0);
	self->m_classType = 0;
	return (PyObject*)self;
}

//paf::ErrorCode GetPrimitiveOrEnum(PyObject*& pyObject, paf::Variant* variant)
//{
//	if (variant->m_type->isPrimitive())
//	{
//		if (variant->byValue() || variant->byRef())
//		{
//			paf::PrimitiveType* primitiveType = static_cast<paf::PrimitiveType*>(variant->m_type);
//			switch (primitiveType->m_typeCategory)
//			{
//			case paf::float_type:
//			case paf::double_type:
//			case paf::long_double_type:
//			{
//				double value;
//				primitiveType->castTo(&value, RuntimeTypeOf<double>::RuntimeType::GetSingleton(), variant->m_pointer);
//				pyObject = PyFloat_FromDouble(value);
//			}
//			break;
//			case paf::bool_type:
//			{
//				bool value;
//				primitiveType->castTo(&value, RuntimeTypeOf<bool>::RuntimeType::GetSingleton(), variant->m_pointer);
//				pyObject = value ? Py_True : Py_False;
//			}
//			break;
//			case paf::unsigned_long_long_type:
//			{
//				unsigned PY_LONG_LONG value;
//				primitiveType->castTo(&value, RuntimeTypeOf<unsigned PY_LONG_LONG>::RuntimeType::GetSingleton(), variant->m_pointer);
//				pyObject = PyLong_FromUnsignedLongLong(value);
//			}
//			break;
//			case paf::long_long_type:
//			{
//				PY_LONG_LONG value;
//				primitiveType->castTo(&value, RuntimeTypeOf<PY_LONG_LONG>::RuntimeType::GetSingleton(), variant->m_pointer);
//				pyObject = PyLong_FromLongLong(value);
//			}
//			break;
//			case paf::unsigned_char_type:
//			case paf::unsigned_short_type:
//			case paf::unsigned_int_type:
//			case paf::unsigned_long_type:
//			{
//				unsigned long value;
//				primitiveType->castTo(&value, RuntimeTypeOf<unsigned long>::RuntimeType::GetSingleton(), variant->m_pointer);
//				pyObject = PyLong_FromUnsignedLong(value);
//			}
//			break;
//			default:
//			{
//				long value;
//				primitiveType->castTo(&value, RuntimeTypeOf<long>::RuntimeType::GetSingleton(), variant->m_pointer);
//				pyObject = PyLong_FromLong(value);
//			}
//			}
//			return paf::ErrorCode::s_ok;
//		}
//		else
//		{
//			paf::PrimitiveType* primitiveType = static_cast<paf::PrimitiveType*>(variant->m_type);
//			if (paf::char_type == primitiveType->m_typeCategory)
//			{
//				pyObject = PyUnicode_FromString((const char*)variant->m_pointer);
//				return paf::ErrorCode::s_ok;
//			}
//			else if (paf::wchar_type == primitiveType->m_typeCategory)
//			{
//				pyObject = PyUnicode_FromWideChar((const wchar_t*)variant->m_pointer, -1);
//				return paf::ErrorCode::s_ok;
//			}
//		}
//	}
//	else if (variant->m_type->isEnum())
//	{
//		if (variant->byValue() || variant->byRef())
//		{
//			long value;
//			variant->castToPrimitive(RuntimeTypeOf<long>::RuntimeType::GetSingleton(), &value);
//			pyObject = PyLong_FromLong(value);
//			return paf::ErrorCode::s_ok;
//		}
//	}
//	return paf::e_invalid_type;
//}
//
//paf::ErrorCode SetPrimitiveOrEnum(paf::Variant* variant, PyObject* pyAttr)
//{
//	if (variant->isConstant())
//	{
//		return paf::e_field_is_constant;
//	}
//	paf::Variant value;
//	paf::Variant* attr = PythonToVariant(&value, pyAttr);
//	if (!attr->castToPrimitive(variant->m_type, variant->m_pointer))
//	{
//		return paf::e_invalid_property_type;
//	}
//	return paf::ErrorCode::s_ok;
//}

PyObject* VariantToPython(paf::Variant* variant)
{
	if (variant->isNull())
	{
		return Py_None;
	}
	else if (variant->isValue() || variant->isReference())
	{
		paf::Type* type = variant->getType();
		if (type->isPrimitive())
		{
			paf::PrimitiveType* primitiveType = static_cast<paf::PrimitiveType*>(type);
			switch (primitiveType->getPrimitiveTypeCategory())
			{
			case paf::float_type:
			case paf::double_type:
			case paf::long_double_type: {
				double value;
				variant->castToPrimitive<double>(value);
				return PyFloat_FromDouble(value);
				break; }
			case paf::bool_type: {
				bool value;
				variant->castToPrimitive<bool>(value);
				return value ? Py_True : Py_False;
				break; }
			default: {
				long long value;
				variant->castToPrimitive<long long>(value);
				return PyLong_FromLongLong(value);
				break; }
			}
		}
		else if (type->isEnumeration())
		{
			paf::EnumType* enumType = static_cast<paf::EnumType*>(type);
			long value;
			enumType->cast(RuntimeTypeOf<long>::RuntimeType::GetSingleton(), &value, variant->getRawPointer());
			return PyLong_FromLong(value);
		}
	}
	PyObject* pyObject = VariantWrapper_new(&s_VariantWrapper_Type, 0, 0);
	void* p = ((VariantWrapper*)pyObject)->m_var;
	new(p)paf::Variant(std::move(*variant));
	return pyObject;
}

paf::Variant* PythonToVariant(paf::Variant* value, PyObject* pyObject)
{
	paf::Variant* res = value;
	PyObject* type = PyObject_Type(pyObject); 

	if(PyObject_TypeCheck(pyObject, &s_VariantWrapper_Type))
	{
		res = (paf::Variant*)((VariantWrapper*)pyObject)->m_var;
	}
	else if (Py_IsNone(pyObject))
	{
		value->clear();
	}
	else if(PyBool_Check(pyObject))
	{
		bool b = (pyObject != Py_False);
		value->assignValue<bool>(b);
	}
	else if(PyLong_Check(pyObject))
	{
		PY_LONG_LONG i = PyLong_AsLongLong(pyObject);
		value->assignValue<PY_LONG_LONG>(i);
	}
	else if(PyFloat_Check(pyObject))
	{
		double d = PyFloat_AsDouble(pyObject);
		value->assignValue<double>(d);
	}
	else if(PyUnicode_Check(pyObject))
	{
		Py_ssize_t size;
		const char* s = PyUnicode_AsUTF8AndSize(pyObject, &size);
		value->newValue<paf::String>(s);
	}
	return res;
}

paf::ErrorCode GetStaticField(PyObject*& pyObject, paf::StaticField* field)
{
	//paf::Variant value;
	//if(field->isArray())
	//{
	//	value.assignArray(field->m_type, (void*)field->m_address, field->m_arraySize, field->m_constant, ::paf::Variant::by_array);
	//}
	//else
	//{
	//	value.assignPtr(field->m_type, (void*)field->m_address, field->m_constant, ::paf::Variant::by_ref);
	//}
	//pyObject = VariantToPython(&value);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode SetStaticField(paf::StaticField* field, PyObject* pyAttr)
{
	//if(field->isArray())
	//{
	//	return paf::e_field_is_an_array;
	//}
	//if(field->isConstant())
	//{
	//	return paf::e_field_is_constant;
	//}
	//paf::Variant value;
	//paf::Variant* attr = PythonToVariant(&value, pyAttr);
	//if(!attr->castToObject(field->m_type, (void*)field->m_address))
	//{
	//	return paf::e_invalid_field_type;
	//}
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetInstanceField(PyObject*& pyObject, paf::Variant* that, paf::InstanceField* field)
{
	//size_t baseOffset;
	//if(!static_cast<paf::ClassType*>(that->m_type)->getClassOffset(baseOffset, field->m_objectType))
	//{
	//	return paf::e_invalid_type;
	//}
	//size_t fieldAddress = (size_t)that->m_pointer + baseOffset + field->m_offset;
	//paf::Variant value;

	//if(field->isArray())
	//{
	//	value.assignArray(field->m_type, (void*)fieldAddress, field->m_arraySize, field->m_constant, ::paf::Variant::by_array);
	//}
	//else
	//{
	//	value.assignPtr(field->m_type, (void*)fieldAddress, field->m_constant, ::paf::Variant::by_ref);
	//}
	//pyObject = VariantToPython(&value);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode SetInstanceField(paf::Variant* that, paf::InstanceField* field, PyObject* pyAttr)
{
	//if(field->isArray())
	//{
	//	return paf::e_field_is_an_array;
	//}
	//if(field->isConstant())
	//{
	//	return paf::e_field_is_constant;
	//}
	//size_t baseOffset;
	//if(!static_cast<paf::ClassType*>(that->m_type)->getClassOffset(baseOffset, field->m_objectType))
	//{
	//	return paf::e_invalid_object_type;
	//}
	//size_t fieldAddress = (size_t)that->m_pointer + baseOffset + field->m_offset;
	//paf::Variant value;
	//paf::Variant* attr = PythonToVariant(&value, pyAttr);
	//if(!attr->castToObject(field->m_type, (void*)fieldAddress))
	//{
	//	return paf::e_invalid_field_type;
	//}
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetInstanceProperty(PyObject*& pyObject, paf::Variant* that, paf::InstanceProperty* property)
{
	assert(paf::PropertyCategory::simple_property == property->get_propertyCategory());
	if (nullptr == property->m_simpleGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_simpleGet)(property, that, &value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		pyObject = VariantToPython(&value);
	}
	return errorCode;
}

paf::ErrorCode SetInstanceProperty(paf::Variant* that, paf::InstanceProperty* property, PyObject* pyAttr)
{
	assert(paf::PropertyCategory::simple_property == property->get_propertyCategory());
	if (nullptr == property->m_simpleSet)
	{
		return paf::ErrorCode::e_property_set_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = PythonToVariant(&value, pyAttr);
	paf::ErrorCode errorCode = (*property->m_simpleSet)(property, that, arg);
	return errorCode;
}

paf::ErrorCode GetStaticProperty(PyObject*& pyObject, paf::StaticProperty* property)
{
	if (nullptr == property->m_simpleGet)
	{
		return paf::ErrorCode::e_property_get_not_implemented;
	}
	paf::Variant value;
	paf::ErrorCode errorCode = (*property->m_simpleGet)(property, &value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		pyObject = VariantToPython(&value);
	}
	return errorCode;
}

paf::ErrorCode SetStaticProperty(paf::StaticProperty* property, PyObject* pyAttr)
{
	if (nullptr == property->m_simpleSet)
	{
		return paf::ErrorCode::e_property_set_not_implemented;
	}
	paf::Variant value;
	paf::Variant* arg = PythonToVariant(&value, pyAttr);
	paf::ErrorCode errorCode = (*property->m_simpleSet)(property, arg);
	return errorCode;
}

paf::ErrorCode MakeMethodInvoker(PyObject*& pyObject, paf::InvokeMethod invoker)
{
	PyObject* object = FunctionInvokerWrapper_new(&s_FunctionInvokerWrapper_Type, 0, 0);
	((FunctionInvokerWrapper*)object)->m_invoker = invoker;
	pyObject = object;
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode MakeInstanceMethod(PyObject*& pyObject, VariantWrapper* self, paf::InstanceMethod* method)
{
	PyObject* object = InstanceMethodWrapper_new(&s_InstanceMethodWrapper_Type, 0, 0);
	((InstanceMethodWrapper*)object)->m_method = method;
	((InstanceMethodWrapper*)object)->m_self = self;
	Py_INCREF(self);
	pyObject = object;
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode GetNestedType(PyObject*& pyObject, paf::Type* nestedType)
{
	paf::Variant value;
	value.assignRawPtr(RuntimeTypeOf<paf::Type>::RuntimeType::GetSingleton(), nestedType);
	pyObject = VariantToPython(&value);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode MakeProxyCreator(PyObject*& pyObject, paf::ClassType* classType)
{
	PyObject* object = ProxyCreatorWrapper_new(&s_ProxyCreatorWrapper_Type, 0, 0);
	((ProxyCreatorWrapper*)object)->m_classType = classType;
	pyObject = object;
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode Variant_GetAttr(PyObject*& pyObject, VariantWrapper* self, const char *name)
{
	paf::Variant* variant = (paf::Variant*)self->m_var;
	switch (variant->getType()->getMetaCategory())
	{
	case paf::MetaCategory::name_space: {
		paf::NameSpace* ns = (paf::NameSpace*)variant->getRawPointer();
		paf::Metadata* member = ns->findMember(name);
		if (0 != member)
		{
			paf::Variant value;
			value.assignRawPtr(member);
			pyObject = VariantToPython(&value);
			return paf::ErrorCode::s_ok;
		}
		break; }
	case paf::MetaCategory::object_type: {
		paf::ClassType* type = (paf::ClassType*)variant->getRawPointer();
		paf::Metadata* member = type->findClassMember(name, true, true);
		if (0 != member)
		{
			paf::Type* memberType = member->getType();
			switch (memberType->getMetaCategory())
			{
			case paf::MetaCategory::static_field:
				return GetStaticField(pyObject, static_cast<paf::StaticField*>(member));
			case paf::MetaCategory::static_property:
				switch (static_cast<paf::StaticProperty*>(member)->get_propertyCategory())
				{
				case paf::PropertyCategory::simple_property:
					return GetStaticProperty(pyObject, static_cast<paf::StaticProperty*>(member));
				case paf::PropertyCategory::array_property:
					//return MakeStaticArrayProperty(pyObject, static_cast<paf::StaticProperty*>(member));
				case paf::PropertyCategory::collection_property:
					return paf::ErrorCode::s_ok;//return MakeStaticMapProperty(pyObject, static_cast<paf::StaticProperty*>(member));
				}
			case paf::MetaCategory::static_method:
				return MakeMethodInvoker(pyObject, static_cast<paf::StaticMethod*>(member)->m_invokeMethod);
			case paf::MetaCategory::enumeration_type:
			case paf::MetaCategory::object_type:
				return GetNestedType(pyObject, static_cast<paf::Type*>(member));
			}
		}
		break; }
	case paf::MetaCategory::enumeration_type: {
		paf::EnumType* et = (paf::EnumType*)variant->getRawPointer();
		paf::Enumerator* enumerator = et->findEnumerator(name);
		if (0 != enumerator)
		{
			paf::Variant value;
			value.assignEnumByInt(et, enumerator->get__value_());
			pyObject = VariantToPython(&value);
			return paf::ErrorCode::s_ok;
		}
		break; }
	}
	paf::Metadata* member;
	member = variant->getType()->findMember(name);
	if(0 != member)
	{
		paf::Type* memberType = member->getType();
		switch (memberType->getMetaCategory())
		{
		case paf::MetaCategory::instance_field:
			return GetInstanceField(pyObject, variant, static_cast<paf::InstanceField*>(member));
		case paf::MetaCategory::static_field:
			return GetStaticField(pyObject, static_cast<paf::StaticField*>(member));
		case paf::MetaCategory::instance_property:
			return GetInstanceProperty(pyObject, variant, static_cast<paf::InstanceProperty*>(member));
		case paf::MetaCategory::static_property:
			return GetStaticProperty(pyObject, static_cast<paf::StaticProperty*>(member));
		case paf::MetaCategory::instance_method:
			return MakeInstanceMethod(pyObject, self, static_cast<paf::InstanceMethod*>(member));
		case paf::MetaCategory::static_method:
			return MakeMethodInvoker(pyObject, static_cast<paf::StaticMethod*>(member)->m_invokeMethod);
		case paf::MetaCategory::enumeration_type:
		case paf::MetaCategory::object_type:
			return GetNestedType(pyObject, static_cast<paf::Type*>(member));
		default:
			assert(false);
		}
	}
	else if (name[0] == '_')
	{
		switch (name[1])
		{
		case 'D':
			if (strcmp(&name[2], "erive_") == 0)//_Derive_
			{
				if (paf::MetaCategory::object_type == variant->getType()->getMetaCategory())
				{
					paf::ClassType* classType = (paf::ClassType*)variant->getRawPointer();
					return MakeProxyCreator(pyObject, classType);
				}
				else
				{
					return paf::ErrorCode::e_is_not_class;
				}
			}
			break;
		case 'a':
			if (strcmp(&name[2], "ddress_") == 0)//_address_
			{
				pyObject = PyLong_FromVoidPtr(variant->getRawPointer());
				return paf::ErrorCode::s_ok;
			}
			break;
		case 'c':
			if (strcmp(&name[2], "ount_") == 0)//_count_
			{
				pyObject = PyLong_FromSize_t(variant->getArraySize());
				return paf::ErrorCode::s_ok;
			}
			break;
		case 's':
			if (strcmp(&name[2], "ize_") == 0)//_size_
			{
				pyObject = PyLong_FromSize_t(variant->getType()->get__size_());
				return paf::ErrorCode::s_ok;
			}
			break;
		case 't':
			if (strcmp(&name[2], "ype_") == 0)//_type_
			{
				paf::Variant typeVar;
				typeVar.assignRawPtr(RuntimeTypeOf<paf::Type>::RuntimeType::GetSingleton(), variant->getType());
				pyObject = VariantToPython(&typeVar);
				return paf::ErrorCode::s_ok;
			}
			break;
		}
	}
	return paf::ErrorCode::e_member_not_found;
}


paf::ErrorCode Variant_SetAttr(paf::Variant* variant, char* name, PyObject* pyAttr)
{
	switch (variant->getType()->getMetaCategory())
	{
	case paf::MetaCategory::object_type: {
		paf::ClassType* type = (paf::ClassType*)variant->getRawPointer();
		paf::Metadata* member = type->findClassMember(name, true, true);
		if (0 != member)
		{
			paf::Type* memberType = member->getType();
			switch (memberType->getMetaCategory())
			{
			case paf::MetaCategory::static_field:
				return SetStaticField(static_cast<paf::StaticField*>(member), pyAttr);
			case paf::MetaCategory::static_property:
				return SetStaticProperty(static_cast<paf::StaticProperty*>(member), pyAttr);
			}
		}
		break; }
	}
	paf::Metadata* member;
	member = variant->getType()->findMember(name);
	if(0 != member)
	{
		paf::Type* memberType = member->getType();
		switch (memberType->getMetaCategory())
		{
		case paf::MetaCategory::instance_field:
			return SetInstanceField(variant, static_cast<paf::InstanceField*>(member), pyAttr);
		case paf::MetaCategory::static_field:
			return SetStaticField(static_cast<paf::StaticField*>(member), pyAttr);
		case paf::MetaCategory::instance_property:
			return SetInstanceProperty(variant, static_cast<paf::InstanceProperty*>(member), pyAttr);
		case paf::MetaCategory::static_property:
			return SetStaticProperty(static_cast<paf::StaticProperty*>(member), pyAttr);
		}
	}
	return paf::ErrorCode::e_member_not_found;
}

paf::ErrorCode Variant_Subscript(PyObject*& pyObject, VariantWrapper* wrapper, size_t index)
{
	paf::Variant* variant = (paf::Variant*)wrapper->m_var;
	paf::Variant item;
	paf::ErrorCode errorCode = variant->subscript(item, index);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	pyObject = VariantToPython(&item);
	return paf::ErrorCode::s_ok;
}

paf::ErrorCode Variant_AssignSubscript(VariantWrapper* wrapper, size_t index, PyObject* pyAttr)
{
	paf::Variant* variant = (paf::Variant*)wrapper->m_var;
	paf::Variant item;
	paf::ErrorCode errorCode = variant->subscript(item, index);
	if (paf::ErrorCode::s_ok != errorCode)
	{
		return errorCode;
	}
	PAF_ASSERT(item.isReference());
	paf::Variant value;
	paf::Variant* attr = PythonToVariant(&value, pyAttr);
	//if (!attr->castToObject(item.m_type, item.m_pointer))
	//{
	//	return paf::e_invalid_type;
	//}
	return paf::ErrorCode::s_ok;
}

PyObject* VariantWrapper_getattr(VariantWrapper* wrapper, char* name)
{
	PyObject* res = 0;
	paf::ErrorCode errorCode = Variant_GetAttr(res, wrapper, name);
	if(paf::ErrorCode::s_ok != errorCode)
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	}
	return res;
}

int VariantWrapper_setattr(VariantWrapper* wrapper, char* name, PyObject* pyAttr)
{
	paf::Variant* self = (paf::Variant*)wrapper->m_var;
	paf::ErrorCode errorCode = Variant_SetAttr(self, name, pyAttr);
	if(paf::ErrorCode::s_ok != errorCode)
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
		return 1;
	}
	return 0;
}

PyObject* VariantWrapper_call(VariantWrapper* wrapper, PyObject* parameters, PyObject*)
{
	paf::InvokeMethod invoker = 0;
	paf::Variant* variant = (paf::Variant*)wrapper->m_var;
	switch (variant->getType()->getMetaCategory())
	{
	case paf::MetaCategory::static_method: {
		paf::StaticMethod* method = (paf::StaticMethod*)variant->getRawPointer();
		invoker = method->m_invokeMethod;
		break; }
	case paf::MetaCategory::instance_method: {
		paf::InstanceMethod* method = (paf::InstanceMethod*)variant->getRawPointer();
		invoker = method->m_invokeMethod;
		break; }
	case paf::MetaCategory::primitive_type:
	case paf::MetaCategory::enumeration_type:
	case paf::MetaCategory::object_type: {
		paf::Type* type = (paf::Type*)variant->getRawPointer();
		//int numArgs = lua_gettop(L) - 1;
		//return Type_New(L, type, &::paf::Variant::newValue, numArgs, 2);
		break; }
	}
	
	if(0 != invoker)
	{
		paf::Variant arguments[max_param_count];
		paf::Variant* args[max_param_count]; 
		Py_ssize_t tupleSize = PyTuple_Size(parameters);
		uint32_t numArgs = uint32_t(tupleSize) < max_param_count ? uint32_t(tupleSize) : max_param_count;
		for(uint32_t i = 0; i < numArgs; ++i)
		{
			PyObject* pyArg = PyTuple_GetItem(parameters, i); 
			args[i] = PythonToVariant(&arguments[i], pyArg);
		}
		paf::Variant results[max_result_count];
		uint32_t resultCount = max_result_count;
		paf::ErrorCode errorCode = (*invoker)(results, resultCount, args, numArgs);
		//for (int i = 0; i < numArgs; i++)
		//{
		//	paf::Variant* argument = (paf::Variant*)&argumentsBuf[sizeof(paf::Variant)*i];
		//	argument->~Variant();
		//}
		if (paf::ErrorCode::s_ok == errorCode)
		{
			if (0 == resultCount)
			{
				return nullptr;
			}
			else if (1 == resultCount)
			{
				PyObject* pyObject = VariantToPython(&results[0]);
				return pyObject;
			}
			else
			{
				for (uint32_t i = 0; i < resultCount; ++i)
				{
					//VariantToLua(L, &results[i]);
				}
				return nullptr;
			}
		}
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	}
	else
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::ErrorCode::e_is_not_type));
	}
	return 0;
}

PyObject* VariantWrapper_str(VariantWrapper* wrapper)
{
	PyObject* pyObject = nullptr;
	paf::Variant* variant = (paf::Variant*)wrapper->m_var;
	PAF_ASSERT(!variant->isNull());
	paf::string_t str;
	if (variant->castToValue(str))
	{
		pyObject = PyUnicode_FromString(str.c_str());
	}
	return pyObject;
}

Py_ssize_t VariantWrapper_length(VariantWrapper* wrapper)
{
	paf::Variant* variant = (paf::Variant*)wrapper->m_var;
	if (variant->isArray())
	{
		return variant->getArraySize();
	}
	else
	{
		return 0;
	}
}

bool VariantWrapper_ParseSubscript(size_t& num, PyObject* pyObject)
{
	PyObject* type = PyObject_Type(pyObject); 
	if(PyObject_TypeCheck(pyObject, &s_VariantWrapper_Type))
	{
		paf::Variant* variant = (paf::Variant*)((VariantWrapper*)pyObject)->m_var;
		if(variant->castToPrimitive(RuntimeTypeOf<size_t>::RuntimeType::GetSingleton(), &num))
		{
			return true;
		}
	}
	if(PyLong_Check(pyObject))
	{
		num = PyLong_AsSize_t(pyObject);
		return true;
	}
	return false;
}

PyObject* VariantWrapper_subscript(VariantWrapper* wrapper, PyObject* subscript)
{
	size_t index;
	if(!VariantWrapper_ParseSubscript(index, subscript))
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::ErrorCode::e_invalid_subscript_type));
		return 0;
	}
	PyObject* res = 0;
	paf::ErrorCode errorCode = Variant_Subscript(res, wrapper, index);
	if(paf::ErrorCode::s_ok != errorCode)
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	}
	return res;
}

int VariantWrapper_assign_subscript(VariantWrapper* wrapper, PyObject* subscript, PyObject* value)
{
	size_t index;
	if(!VariantWrapper_ParseSubscript(index, subscript))
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::ErrorCode::e_invalid_subscript_type));
		return 1;
	}
	paf::ErrorCode errorCode = Variant_AssignSubscript(wrapper, index, value);
	if(paf::ErrorCode::s_ok != errorCode)
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
		return 1;
	}
	return 0;
}

PyObject* InstanceArrayPropertyWrapper_getattr(InstanceArrayPropertyWrapper* wrapper, char* name)
{
	PyObject* res = 0;
	//paf::ErrorCode errorCode;
	//if (strcmp("_count_", name) == 0)
	//{
	//	errorCode = GetInstanceArrayPropertySize(res, (paf::Variant*)wrapper->m_self->m_var, wrapper->m_property);
	//}
	//else
	//{
	//	errorCode = paf::e_member_not_found;
	//}
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//}
	return res;
}

int InstanceArrayPropertyWrapper_setattr(InstanceArrayPropertyWrapper* wrapper, char* name, PyObject* pyAttr)
{
	//paf::ErrorCode errorCode;
	//if (strcmp("_count_", name) == 0)
	//{
	//	errorCode = SetInstanceArrayPropertySize((paf::Variant*)wrapper->m_self->m_var, wrapper->m_property, pyAttr);
	//}
	//else
	//{
	//	errorCode = paf::e_member_not_found;
	//}
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//	return 1;
	//}
	return 0;
}

Py_ssize_t InstanceArrayPropertyWrapper_length(InstanceArrayPropertyWrapper* wrapper)
{
	paf::ErrorCode errorCode;
	size_t value;
	errorCode = (*wrapper->m_property->m_arraySize)(wrapper->m_property, (paf::Variant*)wrapper->m_self->m_var, value);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		return Py_ssize_t(value);
	}
	else
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
		return 0;
	}
}

PyObject* InstanceArrayPropertyWrapper_subscript(InstanceArrayPropertyWrapper* wrapper, PyObject* subscript)
{
	//size_t index;
	//if (!VariantWrapper_ParseSubscript(index, subscript))
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::e_invalid_subscript_type));
	//	return 0;
	//}
	//PyObject* res = 0;
	//paf::ErrorCode errorCode = GetInstanceArrayProperty(res, (paf::Variant*)wrapper->m_self->m_var, wrapper->m_property, index);
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//}
	//return res;
	return nullptr;
}

int InstanceArrayPropertyWrapper_assign_subscript(InstanceArrayPropertyWrapper* wrapper, PyObject* subscript, PyObject* value)
{
	//size_t index;
	//if (!VariantWrapper_ParseSubscript(index, subscript))
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::e_invalid_subscript_type));
	//	return 1;
	//}
	//paf::ErrorCode errorCode = SetInstanceArrayProperty((paf::Variant*)wrapper->m_self->m_var, wrapper->m_property, index, value);
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//	return 1;
	//}
	return 0;
}

PyObject* StaticArrayPropertyWrapper_getattr(StaticArrayPropertyWrapper* wrapper, char* name)
{
	PyObject* res = 0;
	//paf::ErrorCode errorCode;
	//if (strcmp("_count_", name) == 0)
	//{
	//	errorCode = GetStaticArrayPropertySize(res, wrapper->m_property);
	//}
	//else
	//{
	//	errorCode = paf::e_member_not_found;
	//}
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//}
	return res;
}

int StaticArrayPropertyWrapper_setattr(StaticArrayPropertyWrapper* wrapper, char* name, PyObject* pyAttr)
{
	//paf::ErrorCode errorCode;
	//if (strcmp("_count_", name) == 0)
	//{
	//	errorCode = SetStaticArrayPropertySize(wrapper->m_property, pyAttr);
	//}
	//else
	//{
	//	errorCode = paf::e_member_not_found;
	//}
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//	return 1;
	//}
	return 0;
}

Py_ssize_t StaticArrayPropertyWrapper_length(StaticArrayPropertyWrapper* wrapper)
{
	paf::ErrorCode errorCode;
	size_t size;
	errorCode = (*wrapper->m_property->m_arraySize)(wrapper->m_property, size);
	if (paf::ErrorCode::s_ok == errorCode)
	{
		return Py_ssize_t(size);
	}
	else
	{
		PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
		return 0;
	}
}

PyObject* StaticArrayPropertyWrapper_subscript(StaticArrayPropertyWrapper* wrapper, PyObject* subscript)
{
	//size_t index;
	//if (!VariantWrapper_ParseSubscript(index, subscript))
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::e_invalid_subscript_type));
	//	return 0;
	//}
	//PyObject* res = 0;
	//paf::ErrorCode errorCode = GetStaticArrayProperty(res, wrapper->m_property, index);
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//}
	//return res;
	return nullptr;
}

int StaticArrayPropertyWrapper_assign_subscript(StaticArrayPropertyWrapper* wrapper, PyObject* subscript, PyObject* value)
{
	//size_t index;
	//if (!VariantWrapper_ParseSubscript(index, subscript))
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::e_invalid_subscript_type));
	//	return 1;
	//}
	//paf::ErrorCode errorCode = SetStaticArrayProperty(wrapper->m_property, index, value);
	//if (paf::ErrorCode::s_ok != errorCode)
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//	return 1;
	//}
	return 0;
}

PyObject* InstanceMethodWrapper_call(InstanceMethodWrapper* wrapper, PyObject* parameters, PyObject*)
{
	paf::InstanceMethod* method = wrapper->m_method;
	VariantWrapper* self = wrapper->m_self;

	paf::Variant arguments[max_param_count];
	paf::Variant* args[max_param_count + 1]; 

	args[0] = (paf::Variant*)self->m_var;
	Py_ssize_t tupleSize = PyTuple_Size(parameters);
	uint32_t numArgs = uint32_t(tupleSize) < max_param_count ? uint32_t(tupleSize) : max_param_count;
	for(Py_ssize_t i = 0; i < numArgs; ++i)
	{
		PyObject* pyArg = PyTuple_GetItem(parameters, i); 
		args[i+1] = PythonToVariant(&arguments[i], pyArg);
	}
	paf::Variant results[max_result_count];
	uint32_t resultCount = max_result_count;
	paf::ErrorCode errorCode = (*method->m_invokeMethod)(results, resultCount, args, numArgs + 1);
	if(paf::ErrorCode::s_ok == errorCode)
	{
		PyObject* pyObject = VariantToPython(&results[0]);
		return pyObject;
	}
	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	return 0;
}

PyObject* FunctionInvokerWrapper_call(FunctionInvokerWrapper* wrapper, PyObject* parameters, PyObject*)
{
	paf::InvokeMethod invoker = wrapper->m_invoker;

	paf::Variant arguments[max_param_count];
	paf::Variant* args[max_param_count]; 

	Py_ssize_t tupleSize = PyTuple_Size(parameters);
	uint32_t numArgs = uint32_t(tupleSize) < max_param_count ? uint32_t(tupleSize) : max_param_count;

	for(Py_ssize_t i = 0; i < numArgs; ++i)
	{
		PyObject* pyArg = PyTuple_GetItem(parameters, i); 
		args[i] = PythonToVariant(&arguments[i], pyArg);
	}
	paf::Variant results[max_result_count];
	uint32_t resultCount = max_result_count;
	paf::ErrorCode errorCode = (*invoker)(results, resultCount, args, numArgs);
	if(paf::ErrorCode::s_ok == errorCode)
	{
		PyObject* pyObject = VariantToPython(&results[0]);
		return pyObject;
	}
	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	return 0;
}

PyObject* ProxyCreatorWrapper_call(ProxyCreatorWrapper* wrapper, PyObject* parameters, PyObject*)
{
	paf::ClassType* classType = wrapper->m_classType;
	Py_ssize_t numArgs = PyTuple_Size(parameters);
	PyObject* pyArg = PyTuple_GetItem(parameters, 0); 
	if(pyArg)
	{
		PythonSubclassInvoker* subclassInvoker = new PythonSubclassInvoker(pyArg);
		paf::SharedPtr<paf::Introspectable> implementor = classType->createSubclassProxy(subclassInvoker);
		paf::Variant impVar;
		impVar.assignSharedPtr(std::move(implementor));
		return VariantToPython(&impVar);
	}
	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::ErrorCode::e_invalid_arg_type_1));
	return 0;
}

PyObject* VariantWrapper_unaryOperator(PyObject* pyObject, const char* op)
{
	//paf::Variant arguments[1];
	//paf::Variant* args[1];
	//args[0] = PythonToVariant(&arguments[0], pyObject);

	//paf::InstanceMethod* method;

	//switch (args[0]->m_type->m_category)
	//{
	//case paf::primitive_object:
	//	{
	//		paf::PrimitiveType* type = (paf::PrimitiveType*)args[0]->m_type;
	//		method = type->findInstanceMethod(op);
	//	}
	//	break;
	//case paf::value_object:
	//case paf::reference_object:
	//	{
	//		paf::ClassType* type = (paf::ClassType*)args[0]->m_type;
	//		method = type->findInstanceMethod(op, true);
	//	}
	//	break;
	//default:
	//	method = 0;
	//};
	//if (0 != method)
	//{
	//	paf::Variant result;
	//	paf::ErrorCode errorCode = (*method->m_invoker)(&result, args, 1);
	//	if (paf::ErrorCode::s_ok == errorCode)
	//	{
	//		PyObject* pyObject = VariantToPython(&result);
	//		return pyObject;
	//	}
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//}
	//else
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::e_member_not_found));
	//}
	return 0;
}

PyObject* VariantWrapper_binaryOperator(PyObject* pyObject1, PyObject* pyObject2, const char* op)
{
	//paf::Variant arguments[2];
	//paf::Variant* args[2];
	//args[0] = PythonToVariant(&arguments[0], pyObject1);
	//args[1] = PythonToVariant(&arguments[1], pyObject2);

	//paf::InstanceMethod* method;

	//switch (args[0]->m_type->m_category)
	//{
	//case paf::primitive_object:
	//	{
	//		paf::PrimitiveType* type = (paf::PrimitiveType*)args[0]->m_type;
	//		method = type->findInstanceMethod(op);
	//	}
	//	break;
	//case paf::value_object:
	//case paf::reference_object:
	//	{
	//		paf::ClassType* type = (paf::ClassType*)args[0]->m_type;
	//		method = type->findInstanceMethod(op, true);
	//	}
	//	break;
	//default:
	//	method = 0;
	//};
	//if (0 != method)
	//{
	//	paf::Variant result;
	//	paf::ErrorCode errorCode = (*method->m_invoker)(&result, args, 2);
	//	if (paf::ErrorCode::s_ok == errorCode)
	//	{
	//		PyObject* pyObject = VariantToPython(&result);
	//		return pyObject;
	//	}
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
	//}
	//else
	//{
	//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(paf::e_member_not_found));
	//}
	return 0;
}

//PyObject* VariantWrapper_compoundAssignmentOperator(PyObject* pyObject1, PyObject* pyObject2, paf::FunctionInvoker invoker)
//{
//	paf::Variant arguments[2];
//	paf::Variant* args[2];
//	args[0] = PythonToVariant(&arguments[0], pyObject1);
//	args[1] = PythonToVariant(&arguments[1], pyObject2);
//	paf::Variant result;
//	paf::ErrorCode errorCode = (*invoker)(&result, args, 2);
//	if(paf::ErrorCode::s_ok == errorCode)
//	{
//		Py_INCREF(pyObject1);
//		return pyObject1;
//	}
//	PyErr_SetString(PyExc_RuntimeError, ErrorCodeToString(errorCode));
//	return 0;
//}

PyObject* VariantWrapper_add(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_add");
}

PyObject* VariantWrapper_subtract(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_subtract");
}

PyObject* VariantWrapper_multiply(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_multiply");
}

PyObject* VariantWrapper_mod(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_mod");
}

PyObject* VariantWrapper_negative(PyObject* pyObject)
{
	return VariantWrapper_unaryOperator(pyObject, "op_negate");
}

PyObject* VariantWrapper_positive(PyObject* pyObject)
{
	return VariantWrapper_unaryOperator(pyObject, "op_plus");
}

PyObject* VariantWrapper_invert(PyObject* pyObject)
{
	return VariantWrapper_unaryOperator(pyObject, "op_bitwiseNot");
}

PyObject* VariantWrapper_lshift(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_leftShift");
}

PyObject* VariantWrapper_rshift(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_rightShift");
}

PyObject* VariantWrapper_and(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_bitwiseAnd");
}

PyObject* VariantWrapper_xor(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_bitwiseXor");
}

PyObject* VariantWrapper_or(PyObject* pyObject1, PyObject* pyObject2)
{
	return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_bitwiseOr");
}
//
//PyObject* VariantWrapper_inplace_add(PyObject* pyObject1, PyObject* pyObject2)
//{
//	return VariantWrapper_compoundAssignmentOperator(pyObject1, pyObject2, paf::PrimitiveType::Primitive_op_addAssign);
//}

static PyNumberMethods s_VariantWrapper_Operators = 
{
	VariantWrapper_add,//binaryfunc nb_add;
	VariantWrapper_subtract,//binaryfunc nb_subtract;
	VariantWrapper_multiply,//binaryfunc nb_multiply;
	VariantWrapper_mod,//binaryfunc nb_remainder;
	0,//binaryfunc nb_divmod;
	0,//ternaryfunc nb_power;
	VariantWrapper_negative,//unaryfunc nb_negative;
	VariantWrapper_positive,//unaryfunc nb_positive;
	0,//unaryfunc nb_absolute;
	0,//inquiry nb_bool;
	VariantWrapper_invert,//unaryfunc nb_invert;
	VariantWrapper_lshift,//binaryfunc nb_lshift;
	VariantWrapper_rshift,//binaryfunc nb_rshift;
	VariantWrapper_and,//binaryfunc nb_and;
	VariantWrapper_xor,//binaryfunc nb_xor;
	VariantWrapper_or,//binaryfunc nb_or;
	0,//unaryfunc nb_int;
	0,//void *nb_reserved;  /* the slot formerly known as nb_long */
	0,//unaryfunc nb_float;
	0,//binaryfunc nb_inplace_add;
	0,//binaryfunc nb_inplace_subtract;
	0,//binaryfunc nb_inplace_multiply;
	0,//binaryfunc nb_inplace_remainder;
	0,//ternaryfunc nb_inplace_power;
	0,//binaryfunc nb_inplace_lshift;
	0,//binaryfunc nb_inplace_rshift;
	0,//binaryfunc nb_inplace_and;
	0,//binaryfunc nb_inplace_xor;
	0,//binaryfunc nb_inplace_or;
	0,//binaryfunc nb_floor_divide;
	0,//binaryfunc nb_true_divide;
	0,//binaryfunc nb_inplace_floor_divide;
	0,//binaryfunc nb_inplace_true_divide;
	0,//unaryfunc nb_index;
	0,//binaryfunc nb_matrix_multiply;
	0,//binaryfunc nb_inplace_matrix_multiply;
};

PyObject* VariantWrapper_richcmp(PyObject* pyObject1, PyObject* pyObject2, int op)
{
	switch(op)
	{
	case Py_LT:
		return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_less");
	case Py_LE:
		return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_lessEqual");
	case Py_EQ:
		return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_equal");
	case Py_NE:
		return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_notEqual");
	case Py_GT:
		return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_greater");
	case Py_GE:
		return VariantWrapper_binaryOperator(pyObject1, pyObject2, "op_greaterEqual");
	}
	return 0;
}

static PyMappingMethods s_VariantWrapper_Array_Methods =
{
	(lenfunc)VariantWrapper_length,
	(binaryfunc)VariantWrapper_subscript,
	(objobjargproc)VariantWrapper_assign_subscript
};

static PyMappingMethods s_InstanceArrayPropertyWrapper_Array_Methods =
{
	(lenfunc)InstanceArrayPropertyWrapper_length,
	(binaryfunc)InstanceArrayPropertyWrapper_subscript,
	(objobjargproc)InstanceArrayPropertyWrapper_assign_subscript
};

static PyMappingMethods s_StaticArrayPropertyWrapper_Array_Methods =
{
	(lenfunc)StaticArrayPropertyWrapper_length,
	(binaryfunc)StaticArrayPropertyWrapper_subscript,
	(objobjargproc)StaticArrayPropertyWrapper_assign_subscript
};


static PyModuleDef s_PafPythonModule = 
{
	PyModuleDef_HEAD_INIT,
	"pafpy",
	0,
	-1,
};

PyObject* PyInit_pafpy_()
{
	PyObject* module;

	s_VariantWrapper_Type.tp_dealloc = (destructor)VariantWrapper_dealloc;
	s_VariantWrapper_Type.tp_getattr = (getattrfunc)VariantWrapper_getattr;
	s_VariantWrapper_Type.tp_setattr = (setattrfunc)VariantWrapper_setattr;
	s_VariantWrapper_Type.tp_as_number = &s_VariantWrapper_Operators;
	s_VariantWrapper_Type.tp_as_mapping = &s_VariantWrapper_Array_Methods;
	s_VariantWrapper_Type.tp_call = (ternaryfunc)VariantWrapper_call;
	s_VariantWrapper_Type.tp_str = (reprfunc)VariantWrapper_str;
	s_VariantWrapper_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	s_VariantWrapper_Type.tp_richcompare = &VariantWrapper_richcmp;
	s_VariantWrapper_Type.tp_new = VariantWrapper_new;

	if (PyType_Ready(&s_VariantWrapper_Type) < 0)
	{
		return NULL;
	}

	s_InstanceArrayPropertyWrapper_Type.tp_dealloc = (destructor)InstanceArrayPropertyWrapper_dealloc;
	s_InstanceArrayPropertyWrapper_Type.tp_getattr = (getattrfunc)InstanceArrayPropertyWrapper_getattr;
	s_InstanceArrayPropertyWrapper_Type.tp_setattr = (setattrfunc)InstanceArrayPropertyWrapper_setattr;
	s_InstanceArrayPropertyWrapper_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	s_InstanceArrayPropertyWrapper_Type.tp_new = InstanceArrayPropertyWrapper_new;
	s_InstanceArrayPropertyWrapper_Type.tp_as_mapping = &s_InstanceArrayPropertyWrapper_Array_Methods;
	if (PyType_Ready(&s_InstanceArrayPropertyWrapper_Type) < 0)
	{
		return NULL;
	}

	s_StaticArrayPropertyWrapper_Type.tp_getattr = (getattrfunc)StaticArrayPropertyWrapper_getattr;
	s_StaticArrayPropertyWrapper_Type.tp_setattr = (setattrfunc)StaticArrayPropertyWrapper_setattr;
	s_StaticArrayPropertyWrapper_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	s_StaticArrayPropertyWrapper_Type.tp_new = StaticArrayPropertyWrapper_new;
	s_StaticArrayPropertyWrapper_Type.tp_as_mapping = &s_StaticArrayPropertyWrapper_Array_Methods;
	if (PyType_Ready(&s_StaticArrayPropertyWrapper_Type) < 0)
	{
		return NULL;
	}

	s_InstanceMethodWrapper_Type.tp_dealloc = (destructor)InstanceMethodWrapper_dealloc;
	s_InstanceMethodWrapper_Type.tp_call = (ternaryfunc)InstanceMethodWrapper_call;
	s_InstanceMethodWrapper_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	s_InstanceMethodWrapper_Type.tp_new = InstanceMethodWrapper_new;
	if (PyType_Ready(&s_InstanceMethodWrapper_Type) < 0)
	{
		return NULL;
	}

	s_FunctionInvokerWrapper_Type.tp_call = (ternaryfunc)FunctionInvokerWrapper_call;
	s_FunctionInvokerWrapper_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	s_FunctionInvokerWrapper_Type.tp_new = FunctionInvokerWrapper_new;
	if (PyType_Ready(&s_FunctionInvokerWrapper_Type) < 0)
	{
		return NULL;
	}

	s_ProxyCreatorWrapper_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	s_ProxyCreatorWrapper_Type.tp_new = ProxyCreatorWrapper_new;
	s_ProxyCreatorWrapper_Type.tp_call = (ternaryfunc)ProxyCreatorWrapper_call;
	if (PyType_Ready(&s_ProxyCreatorWrapper_Type) < 0)
	{
		return NULL;
	}

	module = PyModule_Create(&s_PafPythonModule);
	if (module == NULL)
	{
		return NULL;
	}
	Py_INCREF(&s_VariantWrapper_Type);
	PyObject* root = VariantWrapper_new(&s_VariantWrapper_Type, 0, 0);
	paf::Variant* var = (paf::Variant*)((VariantWrapper*)root)->m_var;
	var->assignRawPtr(RuntimeTypeOf<paf::NameSpace>::RuntimeType::GetSingleton(), paf::NameSpace::GetGlobalNameSpace());
	PyModule_AddObject(module, "pafpy", root);
	return module;
}


END_PAFPY

