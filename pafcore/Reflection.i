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

namespace paf
{
	class(value_object) #PAFCORE_EXPORT Reflection
	{
		static String GetTypeFullName(Type* type);
		static String GetTypeAliasFullName(TypeAlias* typeAlias);
		static Type* GetTypeFromFullName(string_t fullName);
#{
		static String PrimitiveToString(const Variant& value);
		static void StringToPrimitive(Variant& value, PrimitiveType* primitiveType, const char* str);
		
		static String EnumToString(const Variant& value);
		static bool StringToEnum(Variant& value, EnumType* enumType, const char* str);
		
		static bool IsString(ClassType* type);
		static bool IsBuffer(ClassType* type);

		//static String ObjectToString(const Variant& value);
		//static bool StringToObject(Variant& value, ClassType* classType, const char* str);
		//static String InstancePropertyToString(Introspectable* that, InstanceProperty* instanceProperty);
		//static String InstancePropertyToString(Variant* that, InstanceProperty* instanceProperty);
		//static ErrorCode StringToInstanceProperty(Variant& that, InstanceProperty* instanceProperty, const char* str);
		//static ErrorCode StringToInstanceProperty(Variant& that, const char* propertyName, const char* str);

		//static ErrorCode NewPrimitive(Variant& result, PrimitiveType* primitiveType);
		//static ErrorCode NewPrimitive(Variant& result, PrimitiveType* primitiveType, Variant* argument);
		//static ErrorCode NewEnum(Variant& result, EnumType* type);
		//static ErrorCode NewEnum(Variant& result, EnumType* type, Variant* argument);
		//static ErrorCode NewClass(Variant& result, ClassType* type);
		//static ErrorCode NewClass(Variant& result, ClassType* type, Variant* argument);
		//static ErrorCode NewObject(Variant& result, Type* type);
		//static ErrorCode NewObject(Variant& result, Type* type, Variant* argument);

		//static ErrorCode GetInstanceFieldRef(Variant& value, Variant* that, InstanceField* field);
		//static ErrorCode SetInstanceField(Variant* that, InstanceField* field, Variant& value);
		

		static ErrorCode SimpleInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, Variant* value);
		static ErrorCode SimpleInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, Variant* value);
		static ErrorCode ArrayInstancePropertySize(InstanceProperty* instanceProperty, Variant* that, size_t& index);
		static ErrorCode ArrayInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);
		static ErrorCode ArrayInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);
		static ErrorCode CollectionInstancePropertyIterate(InstanceProperty* instanceProperty, Variant* that, UniquePtr<Iterator>& iterator);
		static ErrorCode CollectionInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, Variant* value);
		static ErrorCode CollectionInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, size_t removeCount, Variant* value);

		//static ErrorCode GetInstanceArrayProperty(Variant& value, Variant* that, InstanceArrayProperty* property, size_t index);
		//static ErrorCode CallInstanceMethod(Variant& result, Introspectable* that, const char* methodName, Variant** args, int_t numArgs);
		static ErrorCode CallInstanceMethod(InstanceMethod* method, Variant& result, Variant* that, Variant* arguments, uint32_t numArgs);
		static ErrorCode CallStaticMethod(StaticMethod* method, Variant& result, Variant* arguments, uint32_t numArgs);
#}
	};
}
