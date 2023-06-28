#include "Reflection.h"
#include "Reflection.mh"
#include "Reflection.ic"
#include "Reflection.mc"
#include "Type.h"
#include "TypeAlias.h"
#include "NameSpace.h"

BEGIN_PAF


String Reflection::GetTypeFullName(Type*  type)
{
	const char* localName = type->get__name_();
	size_t totalLength = strlen(localName) + 1;
	const char* scopeNames[64];
	size_t scopeCount = 0;

	Metadata* scope = type->getEnclosing();
	while (scope)
	{
		Metadata* nextScope = 0;
		MetaCategory category = scope->getType()->getMetaCategory();
		if (MetaCategory::name_space == category)
		{
			nextScope = static_cast<NameSpace*>(scope)->getEnclosing();
			if (NameSpace::GetGlobalNameSpace() == scope)
			{
				break;
			}
		}
		else
		{
			PAF_ASSERT(MetaCategory::object_type == category);
			nextScope = static_cast<ClassType*>(scope)->getEnclosing();
		}
		const char* scopeName = scope->get__name_();
		scopeNames[scopeCount++] = scopeName;
		totalLength += strlen(scopeName) + 1;
		scope = nextScope;
	}
	String name;
	name.reserve(totalLength);
	for (size_t i = 0; i < scopeCount; ++i)
	{
		name.append(scopeNames[scopeCount - 1 - i]);
		name.append(".");
	}
	name.append(localName);
	return name;
}

String Reflection::GetTypeAliasFullName(TypeAlias* typeAlias)
{
	const char* localName = typeAlias->get__name_();
	size_t totalLength = strlen(localName) + 1;
	const char* scopeNames[64];
	size_t scopeCount = 0;

	Metadata* scope = typeAlias->m_enclosing;
	while (scope)
	{
		Metadata* nextScope = 0;
		MetaCategory category = scope->getType()->getMetaCategory();
		if (MetaCategory::name_space == category)
		{
			nextScope = static_cast<NameSpace*>(scope)->getEnclosing();
			if (NameSpace::GetGlobalNameSpace() == scope)
			{
				break;
			}
		}
		else
		{
			PAF_ASSERT(MetaCategory::object_type == category);
			nextScope = static_cast<ClassType*>(scope)->getEnclosing();
		}
		const char* scopeName = scope->get__name_();
		scopeNames[scopeCount++] = scopeName;
		totalLength += strlen(scopeName) + 1;
		scope = nextScope;
	}
	String name;
	name.reserve(totalLength);
	for (size_t i = 0; i < scopeCount; ++i)
	{
		name.append(scopeNames[scopeCount - 1 - i]);
		name.append(".");
	}
	name.append(localName);
	return name;
}

Type* Reflection::GetTypeFromFullName(string_t fullName)
{
	String name;
	const char* nameBegin = fullName;
	Metadata* metadata = NameSpace::GetGlobalNameSpace();
	while (metadata && nameBegin)
	{
		const char* dot = strchr(nameBegin, '.');
		if (dot)
		{
			name.assign(nameBegin, dot);
			nameBegin = dot + 1;
		}
		else
		{
			name.assign(nameBegin);
			nameBegin = 0;
		}
		MetaCategory category = metadata->getType()->getMetaCategory();
		if (MetaCategory::name_space == category)
		{
			metadata = static_cast<NameSpace*>(metadata)->findMember(name.c_str());
		}
		else if (MetaCategory::object_type == category)
		{
			metadata = static_cast<ClassType*>(metadata)->findNestedType(name.c_str(), true, true);
		}
		else
		{
			PAF_ASSERT(MetaCategory::primitive_type == category || MetaCategory::enumeration_type == category);
		}
	}
	if (metadata)
	{
		MetaCategory category = metadata->getType()->getMetaCategory();
		if (MetaCategory::name_space != category)
		{
			PAF_ASSERT(MetaCategory::primitive_type == category || MetaCategory::enumeration_type == category || MetaCategory::object_type == category);
			return static_cast<Type*>(metadata);
		}
	}
	return nullptr;
}

String Reflection::PrimitiveToString(const Variant& value)
{
	void* pointer = value.getRawPointer();
	if (nullptr == pointer)
	{
		return String();
	}
	String res;
	char buf[64];
	PAF_ASSERT(value.getType()->isPrimitive());
	PrimitiveType* primitiveType = static_cast<PrimitiveType*>(value.getType());
	switch (primitiveType->getPrimitiveTypeCategory())
	{
	case bool_type:
		res.assign(*reinterpret_cast<const bool*>(pointer) ? "true" : "false");
		break;
	case char_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const char*>(pointer));
		res.assign(buf);
		break;
	case signed_char_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const signed char*>(pointer));
		res.assign(buf);
		break;
	case unsigned_char_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const unsigned char*>(pointer));
		res.assign(buf);
		break;
	case wchar_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const wchar_t*>(pointer));
		res.assign(buf);
		break;
	case short_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const short*>(pointer));
		res.assign(buf);
		break;
	case unsigned_short_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const unsigned short*>(pointer));
		res.assign(buf);
		break;
	case int_type:
		sprintf_s(buf, "%d", *reinterpret_cast<const int*>(pointer));
		res.assign(buf);
		break;
	case unsigned_int_type:
		sprintf_s(buf, "%u", *reinterpret_cast<const unsigned int*>(pointer));
		res.assign(buf);
		break;
	case long_type:
		sprintf_s(buf, "%ld", *reinterpret_cast<const long*>(pointer));
		res.assign(buf);
		break;
	case unsigned_long_type:
		sprintf_s(buf, "%lu", *reinterpret_cast<const unsigned long*>(pointer));
		res.assign(buf);
		break;
	case long_long_type:
		sprintf_s(buf, "%lld", *reinterpret_cast<const long long*>(pointer));
		res.assign(buf);
		break;
	case unsigned_long_long_type:
		sprintf_s(buf, "%llud", *reinterpret_cast<const unsigned long long*>(pointer));
		res.assign(buf);
		break;
	case float_type:
		sprintf_s(buf, "%.8g", *reinterpret_cast<const float*>(pointer));
		res.assign(buf);
		break;
	case double_type:
		sprintf_s(buf, "%.16g", *reinterpret_cast<const double*>(pointer));
		res.assign(buf);
		break;
	case long_double_type:
		sprintf_s(buf, "%.16g", *reinterpret_cast<const long double*>(pointer));
		res.assign(buf);
		break;
	}
	return res;
}

void Reflection::StringToPrimitive(Variant& value, PrimitiveType* primitiveType, const char* str)
{
	switch (primitiveType->getPrimitiveTypeCategory())
	{
	case bool_type: {
		bool val = (0 == strcmp(str, "true")) ? true : false;
		value.assignValue(val);
		break; }

	case char_type: {
		char val = (char)strtol(str, 0, 0);
		value.assignValue(val);
		break; }

	case signed_char_type: {
		signed char val = (signed char)strtol(str, 0, 0);
		value.assignValue(val);
		break; }

	case unsigned_char_type: {
		unsigned char val = (unsigned char)strtoul(str, 0, 0);
		value.assignValue(val);
		break; }

	case wchar_type: {
		wchar_t val = (wchar_t)strtoul(str, 0, 0);
		value.assignValue(val);
		break; }

	case short_type: {
		short val = (short)strtol(str, 0, 0);
		value.assignValue(val);
		break; }

	case unsigned_short_type: {
		unsigned short val = (unsigned short)strtoul(str, 0, 0);
		value.assignValue(val);
		break; }

	case int_type: {
		int val = strtol(str, 0, 0);
		value.assignValue(val);
		break; }

	case unsigned_int_type: {
		unsigned int val = strtoul(str, 0, 0);
		value.assignValue(val);
		break; }

	case long_type: {
		long val = strtol(str, 0, 0);
		value.assignValue(val);
		break; }

	case unsigned_long_type: {
		unsigned long val = strtoul(str, 0, 0);
		value.assignValue(val);
		break; }

	case long_long_type: {
		long long val = strtoll(str, 0, 0);
		value.assignValue(val);
		break; }

	case unsigned_long_long_type: {
		unsigned long long val = strtoull(str, 0, 0);
		value.assignValue(val);
		break; }

	case float_type: {
		float val = (float)atof(str);
		value.assignValue(val);
		break; }

	case double_type: {
		double val = atof(str);
		value.assignValue(val);
		break; }

	case long_double_type: {
		long double val = atof(str);
		value.assignValue(val);
		break; }
	}
}

String Reflection::EnumToString(const Variant& value)
{
	Type* type = value.getType();
	if (!type->isEnumeration())
	{
		return String();
	}
	EnumType* enumType = static_cast<EnumType*>(type);
	int enumValue = 0;
	value.castToValue(enumType, &enumValue);
	Enumerator* enumerator = enumType->_getEnumeratorByValue_(enumValue);
	if (enumerator)
	{
		return String(enumerator->get__name_());
	}
	return String();
}

bool Reflection::StringToEnum(Variant& value, EnumType* enumType, const char* str)
{
	Enumerator* enumerator = enumType->findEnumerator(str);
	if (enumerator)
	{
		value.assignEnumByInt(enumType, enumerator->get__value_());
		return true;
	}
	else
	{
		int n = atoi(str);
		value.assignEnumByInt(enumType, n);
		return true;
	}
	return false;
}


//bool Reflection::IsString(ClassType* type)
//{
//	if (string_t::GetType() == type)
//	{
//		return true;
//	}
//	return type->isType(StringBase::GetType());
//}
//
//bool Reflection::IsBuffer(ClassType* type)
//{
//	if (buffer_t::GetType() == type)
//	{
//		return true;
//	}
//	return type->isType(StringBase::GetType());
//}


//String Reflection::ObjectToString(const Variant& value)
//{
//	PAF_ASSERT(value.m_type->isObject() || value.m_type->isIntrospectionObject());
//	ClassType* classType = static_cast<ClassType*>(value.m_type);
//	InstanceMethod* instanceMethod = classType->findInstanceMethod("toString", true);
//	if (instanceMethod)
//	{
//		Variant result;
//		Variant* args = (Variant*)&value;
//		ErrorCode errorCode = (*instanceMethod->m_invokeMethod)(&result, &args, 1);
//		if (s_ok == errorCode && RuntimeTypeOf<String>::RuntimeType::GetSingleton() == result.m_type)
//		{
//			return *reinterpret_cast<String*>(result.m_pointer);
//		}
//	}
//	return String();
//}
//
//bool Reflection::StringToObject(Variant& value, ClassType* classType, const char* str)
//{
//	StaticMethod* staticMethod = classType->findStaticMethod("FromString", true);
//	if (staticMethod)
//	{
//		Variant arg;
//		arg.assignPrimitivePtr(RuntimeTypeOf<char>::RuntimeType::GetSingleton(), str, true, Variant::by_ptr);
//		Variant* args = &arg;
//		ErrorCode errorCode = (*staticMethod->m_invokeMethod)(&value, &args, 1);
//		if (s_ok == errorCode)
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//String Reflection::InstancePropertyToString(Introspectable* that, InstanceProperty* instanceProperty)
//{
//	Variant varThat;
//	varThat.assignReferencePtr(that->getType(), that, false, Variant::by_ptr);
//	return InstancePropertyToString(&varThat, instanceProperty);
//}
//
//String Reflection::InstancePropertyToString(Variant* that, InstanceProperty* instanceProperty)
//{
//	Variant value;
//	ErrorCode errorCode = (*instanceProperty->m_getter)(instanceProperty, that, &value);
//	if (s_ok == errorCode)
//	{
//		if (value.m_type->isPrimitive())
//		{
//			return Reflection::PrimitiveToString(value);
//		}
//		else if (value.m_type->isEnum())
//		{
//			return Reflection::EnumToString(value);
//		}
//		else
//		{
//			PAF_ASSERT(value.m_type->isObject() || value.m_type->isIntrospectionObject());
//			return Reflection::ObjectToString(value);
//		}
//	}
//	return String();
//}
//
//ErrorCode Reflection::StringToInstanceProperty(Variant& that, InstanceProperty* instanceProperty, const char* str)
//{
//	if (0 == instanceProperty->m_setter)
//	{
//		return e_property_is_not_readable;
//	}
//	Variant value;
//	if (instanceProperty->m_type->isPrimitive())
//	{
//		Reflection::StringToPrimitive(value, static_cast<PrimitiveType*>(instanceProperty->m_type), str);
//	}
//	else if (instanceProperty->m_type->isEnum())
//	{
//		Reflection::StringToEnum(value, static_cast<EnumType*>(instanceProperty->m_type), str);
//	}
//	else
//	{
//		PAF_ASSERT(instanceProperty->m_type->isObject() || instanceProperty->m_type->isIntrospectionObject());
//		Reflection::StringToObject(value, static_cast<ClassType*>(instanceProperty->m_type), str);
//	}
//	ErrorCode errorCode = (*instanceProperty->m_setter)(instanceProperty, &that, &value);
//	return errorCode;
//}
//
//ErrorCode Reflection::StringToInstanceProperty(Variant& that, const char* propertyName, const char* str)
//{
//	if (object_type != that.m_type->getType()->getMetaCategory())
//	{
//		return e_is_not_class;
//	}
//	ClassType* classType = static_cast<ClassType*>(that.m_type);
//	InstanceProperty* instanceProperty = classType->findInstanceProperty(propertyName, true);
//	if (0 == instanceProperty)
//	{
//		return e_member_not_found;
//	}
//	return StringToInstanceProperty(that, instanceProperty, str);
//}
//
//
//ErrorCode Reflection::CallInstanceMethod(Variant& result, Introspectable* that, const char* methodName, Variant** args, int_t numArgs)
//{
//	ClassType* classType = that->getType();
//	InstanceMethod* instanceMethod = classType->findInstanceMethod(methodName, true);
//	if (0 == instanceMethod)
//	{
//		return e_member_not_found;
//	}
//
//	ErrorCode errorCode = (*invoker)(&result, args, numArgs);
//	for (int i = 0; i < numArgs; i++)
//	{
//		Variant* argument = (Variant*)&argumentsBuf[sizeof(Variant)*i];
//		argument->~Variant();
//	}
//
//	Metadata* member;
//	member = variant->m_type->findMember(name);
//
//}

//ErrorCode Reflection::AssignValue(Type* dstType, void* dstPtr, Variant& src)
//{
//	if (dstType->isPrimitive())
//	{
//		return src.castToPrimitive(static_cast<PrimitiveType*>(dstType), dstPtr);
//	}
//	else if (dstType->isEnumeration())
//	{
//		return src.castToEnum(static_cast<EnumType*>(dstType), dstPtr);
//	}
//	else if(dstType->isString())
//	{
//		return src.castToString(*(string_t*)dstPtr);
//	}
//	else if (dstType->isBuffer())
//	{
//		return src.castToBuffer(*(buffer_t*)dstPtr);
//	}
//	else
//	{
//		paf::ClassType* classType = static_cast<paf::ClassType*>(dstType);
//		size_t offset;
//		if (classType->getClassOffset(offset, paf::StringBase::GetType()))
//		{
//			paf::StringBase* dstStr = (paf::StringBase*)((size_t)dstPtr + offset);
//			paf::string_t srcStr;
//			if (src.castToString(srcStr))
//			{
//				dstStr->fromString(srcStr);
//				return true;
//			}
//		}
//		else if (classType->getClassOffset(offset, paf::BufferBase::GetType()))
//		{
//			paf::BufferBase* dstBuf = (paf::BufferBase*)((size_t)dstPtr + offset);
//			paf::buffer_t srcBuf;
//			if (src.castToBuffer(srcBuf))
//			{
//				dstBuf->fromBuffer(srcBuf);
//				return true;
//			}
//		}
//		else if (dstType == src.getType())
//		{
//			return classType->copyAssign(dstPtr, src.getRawPointer());
//		}
//	}
//	return false;
//}

ErrorCode Reflection::GetInstanceFieldReference(Variant& result, Variant* that, InstanceField* field)
{
	if (field->isArray())
	{
		return ErrorCode::e_field_is_an_array;
	}
	size_t baseOffset;
	if (!static_cast<ClassType*>(that->getType())->getClassOffset(baseOffset, field->get_objectType()))
	{
		return ErrorCode::e_invalid_type;
	}
	size_t fieldAddress = (size_t)that->getRawPointer() + baseOffset + field->get_offset();
	Type* fieldType = field->get_type();
	switch (field->get_typeCompound())
	{
	case TypeCompound::none:
		result.assignReference(fieldType, (void*)fieldAddress);
		break;
	case TypeCompound::raw_ptr:
	case TypeCompound::borrowed_ptr:
	case TypeCompound::unique_ptr:
	case TypeCompound::shared_ptr:
		result.assignRawPtr(fieldType, reinterpret_cast<GenericPtr*>(fieldAddress)->m_ptr);
		break;
	case TypeCompound::raw_array:
	case TypeCompound::borrowed_array:
	case TypeCompound::unique_array:
	case TypeCompound::shared_array:
		result.assignRawArray(fieldType, reinterpret_cast<GenericArray*>(fieldAddress)->m_ptr, reinterpret_cast<GenericArray*>(fieldAddress)->m_size);
		break;
	default:
		PAF_ASSERT(false);
	}
	return ErrorCode::s_ok;
}

ErrorCode Reflection::SetInstanceField(Variant* that, InstanceField* field, Variant& value)
{
	//if (field->isArray())
	//{
	//	return ErrorCode::e_field_is_an_array;
	//}
	//size_t baseOffset;
	//if (!static_cast<ClassType*>(that->getType())->getClassOffset(baseOffset, field->get_objectType()))
	//{
	//	return ErrorCode::e_invalid_type;
	//}
	//size_t fieldAddress = (size_t)that->getRawPointer() + baseOffset + field->get_offset();
	//Type* fieldType = field->get_type();

	//switch (field->get_typeCompound())
	//{
	//case TypeCompound::none:
	//	if (value.isValue() || value.isReference())
	//	{
	//		return AssignValue(fieldType, (void*)fieldAddress, value);
	//	}
	//	break;
	//case TypeCompound::raw_ptr:
	//case TypeCompound::borrowed_ptr:
	//case TypeCompound::unique_ptr:
	//case TypeCompound::shared_ptr:
	//	result.assignRawPtr(fieldType, reinterpret_cast<GenericPtr*>(fieldAddress)->m_ptr);
	//	break;
	//case TypeCompound::raw_array:
	//case TypeCompound::borrowed_array:
	//case TypeCompound::unique_array:
	//case TypeCompound::shared_array:
	//	result.assignRawArray(fieldType, reinterpret_cast<GenericArray*>(fieldAddress)->m_ptr, reinterpret_cast<GenericArray*>(fieldAddress)->m_size);
	//	break;
	//default:
	//	PAF_ASSERT(false);
	//}
	return ErrorCode::s_ok;
}

ErrorCode Reflection::SimpleInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, Variant* value) 
{
	PAF_ASSERT(instanceProperty && that && value);
	if (PropertyCategory::simple_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_property;
	}
	if (0 == instanceProperty->m_simpleGet)
	{
		return ErrorCode::e_property_get_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_simpleGet)(instanceProperty, that, value);
	return errorCode;
}

ErrorCode Reflection::SimpleInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, Variant* value)
{
	PAF_ASSERT(instanceProperty && that && value);
	if (PropertyCategory::simple_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_property;
	}
	if (0 == instanceProperty->m_simpleSet)
	{
		return ErrorCode::e_property_set_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_simpleSet)(instanceProperty, that, value);
	return errorCode;
}

ErrorCode Reflection::ArrayInstancePropertySize(InstanceProperty* instanceProperty, Variant* that, size_t& size)
{
	PAF_ASSERT(instanceProperty && that);
	if (PropertyCategory::array_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_array_property;
	}
	if (0 == instanceProperty->m_arraySize)
	{
		return ErrorCode::e_property_size_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_arraySize)(instanceProperty, that, size);
	return errorCode;
}

ErrorCode Reflection::ArrayInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value)
{
	PAF_ASSERT(instanceProperty && that && value);
	if (PropertyCategory::array_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_array_property;
	}
	if (0 == instanceProperty->m_arrayGet)
	{
		return ErrorCode::e_property_get_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_arrayGet)(instanceProperty, that, index, value);
	return errorCode;
}

ErrorCode Reflection::ArrayInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value)
{
	PAF_ASSERT(instanceProperty && that && value);
	if (PropertyCategory::array_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_array_property;
	}
	if (0 == instanceProperty->m_arraySet)
	{
		return ErrorCode::e_property_set_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_arraySet)(instanceProperty, that, index, value);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, Variant* value)
{
	PAF_ASSERT(instanceProperty && that && iterator && value);
	if (PropertyCategory::collection_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	if (0 == instanceProperty->m_collectionGet)
	{
		return ErrorCode::e_property_get_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_collectionGet)(instanceProperty, that, iterator, value);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, size_t removeCount, Variant* value)
{
	PAF_ASSERT(instanceProperty && that && iterator && value);
	if (PropertyCategory::collection_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	if (0 == instanceProperty->m_collectionSet)
	{
		return ErrorCode::e_property_set_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_collectionSet)(instanceProperty, that, iterator, removeCount, value);
	return errorCode;
}

ErrorCode Reflection::CollectionInstancePropertyIterate(InstanceProperty* instanceProperty, Variant* that, SharedPtr<Iterator>& iterator)
{
	PAF_ASSERT(instanceProperty && that);
	if (PropertyCategory::collection_property != instanceProperty->get_propertyCategory())
	{
		return ErrorCode::e_is_not_collection_property;
	}
	if (0 == instanceProperty->m_collectionIterate)
	{
		return ErrorCode::e_property_iterate_not_implemented;
	}
	ErrorCode errorCode = (*instanceProperty->m_collectionIterate)(instanceProperty, that, iterator);
	return errorCode;
}

ErrorCode Reflection::CallInstanceMethod(InstanceMethod* method, Variant* results, uint32_t& resultCount, Variant* that, Variant* arguments, uint32_t numArgs)
{
	Variant* args[max_method_param_count + 1];
	if (numArgs > max_method_param_count)
	{
		numArgs = max_method_param_count;
	}
	args[0] = that;
	for (uint32_t i = 0; i < numArgs; ++i)
	{
		args[i + 1] = &arguments[i];
	}
	ErrorCode errorCode = (*method->m_invokeMethod)(results, resultCount, args, numArgs + 1);
	return errorCode;
}

ErrorCode Reflection::CallStaticMethod(StaticMethod* method, Variant* results, uint32_t& resultCount, Variant* arguments, uint32_t numArgs)
{
	Variant* args[max_method_param_count];
	if (numArgs > max_method_param_count)
	{
		numArgs = max_method_param_count;
	}
	for (uint32_t i = 0; i < numArgs; ++i)
	{
		args[i] = &arguments[i];
	}
	ErrorCode errorCode = (*method->m_invokeMethod)(results, resultCount, args, numArgs);
	return errorCode;
}


END_PAF
