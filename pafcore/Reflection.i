#import "Type.i"
#import "TypeAlias.i"
#import "PrimitiveType.i"
#import "EnumType.i"
#import "InstanceField.i"
#import "InstanceProperty.i"
#import "String.i"


#{
#include "Utility.h"
#include "Variant.h"
#include "PrimitiveType.h"
#include "EnumType.h"
#include "InstanceField.h"
#include "InstanceProperty.h"
#include "InstanceMethod.h"
#include "StaticMethod.h"
#}

namespace pafcore
{
	class(value_object) #PAFCORE_EXPORT Reflection
	{
		static String GetTypeFullName(Type* type);
		static String GetTypeAliasFullName(TypeAlias* typeAlias);
		static Type* GetTypeFromFullName(const char* fullName);
#{
		static String PrimitiveToString(const Variant& value);
		static void StringToPrimitive(Variant& value, PrimitiveType* primitiveType, const char* str);
		
		static String EnumToString(const Variant& value);
		static bool StringToEnum(Variant& value, EnumType* enumType, const char* str);
		
		//static String ObjectToString(const Variant& value);
		//static bool StringToObject(Variant& value, ClassType* classType, const char* str);
		//static String InstancePropertyToString(Reference* that, InstanceProperty* instanceProperty);
		//static String InstancePropertyToString(Variant* that, InstanceProperty* instanceProperty);
		//static ErrorCode StringToInstanceProperty(Variant& that, InstanceProperty* instanceProperty, const char* str);
		//static ErrorCode StringToInstanceProperty(Variant& that, const char* propertyName, const char* str);

		static ErrorCode NewPrimitive(Variant& result, PrimitiveType* primitiveType);
		static ErrorCode NewPrimitive(Variant& result, PrimitiveType* primitiveType, Variant* argument);
		static ErrorCode NewEnum(Variant& result, EnumType* type);
		static ErrorCode NewEnum(Variant& result, EnumType* type, Variant* argument);
		static ErrorCode NewClass(Variant& result, ClassType* type);
		static ErrorCode NewClass(Variant& result, ClassType* type, Variant* argument);
		static ErrorCode NewObject(Variant& result, Type* type);
		static ErrorCode NewObject(Variant& result, Type* type, Variant* argument);

		static ErrorCode GetInstanceFieldRef(Variant& value, Variant* that, InstanceField* field);
		static ErrorCode SetInstanceField(Variant* that, InstanceField* field, Variant& value);
		

		static ErrorCode GetInstanceProperty(Variant& value, Variant* that, InstanceProperty* property);
		static ErrorCode SetInstanceProperty(Variant* that, InstanceProperty* property, Variant& value);
		static ErrorCode SimpleInstanceProperty_CandidateCount(Variant& value, Variant* that, InstanceProperty* property);
		static ErrorCode SimpleInstanceProperty_CandidateCount(size_t& size, Variant* that, InstanceProperty* property);
		static ErrorCode SimpleInstanceProperty_GetCandidate(Variant& value, Variant* that, InstanceProperty* property, size_t index);

		static ErrorCode GetArrayInstanceProperty(Variant& value, Variant* that, InstanceProperty* property, size_t index);
		static ErrorCode SetArrayInstanceProperty(Variant* that, InstanceProperty* property, size_t index, Variant& value);
		static ErrorCode GetArrayInstancePropertySize(Variant& value, Variant* that, InstanceProperty* property);
		static ErrorCode GetArrayInstancePropertySize(size_t& size, Variant* that, InstanceProperty* property);
		static ErrorCode ResizeArrayInstanceProperty(Variant* that, InstanceProperty* property, size_t size);
		static ErrorCode ArrayInstanceProperty_GetIterator(Variant* iterator, Variant* that, InstanceProperty* property);
		static ErrorCode ArrayInstanceProperty_GetValue(Variant& value, Variant* that, InstanceProperty* property, Iterator* iterator);


		static ErrorCode GetMapInstanceProperty(Variant& value, Variant* that, InstanceProperty* property, Variant* key);
		static ErrorCode SetMapInstanceProperty(Variant* that, InstanceProperty* property, Variant* key, Variant& value);
		static ErrorCode GetMapInstanceIterator(Variant* iterator, Variant* that, InstanceProperty* property);
		static ErrorCode GetMapInstanceKey(Variant& key, Variant* that, InstanceProperty* property, Iterator* iterator);
		static ErrorCode GetMapInstanceValue(Variant& value, Variant* that, InstanceProperty* property, Iterator* iterator);

		static ErrorCode ListInstanceProperty_PushBack(Variant* that, InstanceProperty* property, Variant& value);
		static ErrorCode ListInstanceProperty_GetIterator(Variant* iterator, Variant* that, InstanceProperty* property);
		static ErrorCode ListInstanceProperty_GetValue(Variant& value, Variant* that, InstanceProperty* property, Iterator* iterator);
		static ErrorCode ListInstanceProperty_Insert(Variant* that, InstanceProperty* property, Iterator* iterator, Variant& value);
		static ErrorCode ListInstanceProperty_Erase(Variant* that, InstanceProperty* property, Iterator* iterator);

		//static ErrorCode GetInstanceArrayProperty(Variant& value, Variant* that, InstanceArrayProperty* property, size_t index);
		//static ErrorCode CallInstanceMethod(Variant& result, Reference* that, const char* methodName, Variant** args, int_t numArgs);
		static ErrorCode CallInstanceMethod(Variant& result, Variant* that, InstanceMethod* method, Variant* arguments, int_t numArgs);
		static ErrorCode CallStaticMethod(Variant& result, StaticMethod* method, Variant* arguments, int_t numArgs);
#}
	};
}
