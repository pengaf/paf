#import "Type.i"
#import "TypeAlias.i"
#import "PrimitiveType.i"
#import "EnumType.i"
#import "Field.i"
#import "Property.i"
#import "String.i"

#{
#include "SmartPtr.h"
#}

namespace paf
{
#{
	class Variant;
	class TypeAlias;
	class PrimitiveType;
	class EnumType;
	class InstanceField;
	class InstanceProperty;
	class StaticMethod;
	class InstanceMethod;
	
	class Iterator;
#}
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
		
		//static bool IsString(ClassType* type);
		//static bool IsBuffer(ClassType* type);
		//static String ObjectToString(const Variant& value);
		//static bool StringToObject(Variant& value, ClassType* classType, const char* str);
		//static String InstancePropertyToString(Introspectable* that, InstanceProperty* instanceProperty);
		//static String InstancePropertyToString(Variant* that, InstanceProperty* instanceProperty);
		//static ErrorCode StringToInstanceProperty(Variant& that, InstanceProperty* instanceProperty, const char* str);
		//static ErrorCode StringToInstanceProperty(Variant& that, const char* propertyName, const char* str);

		//static ErrorCode AssignValue(Type* dstType, void* dstPtr, Variant& src);
		static ErrorCode GetInstanceFieldReference(Variant& result, Variant* that, InstanceField* field);
		static ErrorCode SetInstanceField(Variant* that, InstanceField* field, Variant& value);
		

		static ErrorCode SimpleInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, Variant* value);
		static ErrorCode SimpleInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, Variant* value);
		static ErrorCode ArrayInstancePropertySize(InstanceProperty* instanceProperty, Variant* that, size_t& index);
		static ErrorCode ArrayInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);
		static ErrorCode ArrayInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, size_t index, Variant* value);
		static ErrorCode CollectionInstancePropertyIterate(InstanceProperty* instanceProperty, Variant* that, SharedPtr<Iterator>& iterator);
		static ErrorCode CollectionInstancePropertyGet(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, Variant* value);
		static ErrorCode CollectionInstancePropertySet(InstanceProperty* instanceProperty, Variant* that, Iterator* iterator, size_t removeCount, Variant* value);

		//static ErrorCode GetInstanceArrayProperty(Variant& value, Variant* that, InstanceArrayProperty* property, size_t index);
		//static ErrorCode CallInstanceMethod(Variant& result, Introspectable* that, const char* methodName, Variant** args, int_t numArgs);
		static ErrorCode CallInstanceMethod(InstanceMethod* method, Variant* results, uint32_t& resultCount, Variant* that, Variant* arguments, uint32_t numArgs);
		static ErrorCode CallStaticMethod(StaticMethod* method, Variant* results, uint32_t& resultCount, Variant* arguments, uint32_t numArgs);
#}
	};
}
