#import "Type.i"
#import "InstanceProperty.i"
#import "InstanceField.i"

namespace pafcore
{
#{
	class InstanceField;
	class InstanceProperty;
	class InstanceMethod;
	class StaticField;
	class StaticProperty;
	class StaticArrayProperty;
	class StaticMethod;
	class Enumerator;
	class TypeAlias;
	class SubclassInvoker;
#}
	abstract struct #PAFCORE_EXPORT ClassTypeIterator
	{
		nocode ClassTypeIterator* next();
		nocode ClassType* value();
#{
	public:
		ClassTypeIterator(ClassTypeIterator* iterator, ClassType* classType)
		{
			m_next = iterator;
			m_classType = classType;
		}
		::pafcore::RawPtr<ClassTypeIterator> next()
		{
			return m_next;
		}
		::pafcore::RawPtr<ClassType> value()
		{
			return m_classType;
		}
	protected:
		ClassTypeIterator* m_next;
		ClassType* m_classType;
#}
	};

	abstract class(object_type)#PAFCORE_EXPORT ClassType : Type
	{
		size_t _getMemberCount_(bool includeBaseClasses);
		Metadata* _getMember_(size_t index, bool includeBaseClasses);
		Metadata* _findMember_(string_t name, bool includeBaseClasses);
		size_t _getBaseClassCount_();
		Metadata* _getBaseClass_(size_t index);
		ClassTypeIterator* _getFirstDerivedClass_();
		size_t _getInstancePropertyCount_(bool includeBaseClasses);	
		InstanceProperty* _getInstanceProperty_(size_t index, bool includeBaseClasses);//������property��ǰ	
		size_t _getInstanceFieldCount_(bool includeBaseClasses);
		InstanceField* _getInstanceField_(size_t index, bool includeBaseClasses);//������field��ǰ	
#{
	public:
		struct BaseClass
		{
			ClassType* m_type;
			ptrdiff_t m_offset;
		};
	public:
		ClassType(const char* name, MetaCategory category, const char* declarationFile);
	public:
		virtual Metadata* findMember(const char* name);
		virtual void* createSubclassProxy(SubclassInvoker* subclassInvoker);
		virtual void destroySubclassProxy(void* subclassProxy);
		Metadata* findMember(const char* name, bool includeBaseClasses, bool typeAliasToType);
		Metadata* findClassMember(const char* name, bool includeBaseClasses, bool typeAliasToType);
	public:
		bool isType(ClassType* otherType);
		bool getClassOffset_(size_t& offset, ClassType* otherType);
		bool getClassOffset(size_t& offset, ClassType* otherType);
		Type* findNestedType(const char* name, bool includeBaseClasses, bool typeAliasToType);
		TypeAlias* findNestedTypeAlias(const char* name, bool includeBaseClasses);
		InstanceField* findInstanceField(const char* name, bool includeBaseClasses);
		StaticField* findStaticField(const char* name, bool includeBaseClasses);
		InstanceProperty* findInstanceProperty(const char* name, bool includeBaseClasses);
		StaticProperty* findStaticProperty(const char* name, bool includeBaseClasses);
		InstanceMethod* findInstanceMethod(const char* name, bool includeBaseClasses);
		StaticMethod* findStaticMethod(const char* name, bool includeBaseClasses);
		bool hasDynamicInstanceField(bool includeBaseClasses) const;
		bool hasDynamicInstanceField() const;
	public:
		InstanceProperty* getInstancePropertyBaseClassFirst(size_t index);//����property��ǰ
		InstanceField* getInstanceFieldBaseClassFirst(size_t index);//����field��ǰ
	private:
		InstanceProperty* getInstancePropertyBaseClassFirst_(size_t& index);//����property��ǰ
		InstanceField* getInstanceFieldBaseClassFirst_(size_t& index);//����field��ǰ
		InstanceProperty* getInstanceProperty_(size_t& index);//����property��ǰ
		InstanceField* getInstanceField_(size_t& index);//����field��ǰ
	public:
		BaseClass* m_baseClasses;
		size_t m_baseClassCount;
		ClassTypeIterator* m_classTypeIterators;
		ClassTypeIterator* m_firstDerivedClass;
		Metadata** m_members;
		size_t m_memberCount;
		Type** m_nestedTypes;
		size_t m_nestedTypeCount;
		TypeAlias** m_nestedTypeAliases;
		size_t m_nestedTypeAliasCount;
		InstanceField* m_instanceFields;
		size_t m_instanceFieldCount;
		InstanceProperty* m_instanceProperties;
		size_t m_instancePropertyCount;
		InstanceMethod* m_instanceMethods;
		size_t m_instanceMethodCount;
		StaticField* m_staticFields;
		size_t m_staticFieldCount;
		StaticProperty* m_staticProperties;
		size_t m_staticPropertyCount;
		StaticMethod* m_staticMethods;
		size_t m_staticMethodCount;
		bool m_introspectable;
		bool m_hasDynamicInstanceField;
#}
	};

#{
	inline InstanceProperty* ClassType::getInstancePropertyBaseClassFirst(size_t index)
	{
		InstanceProperty* res = getInstancePropertyBaseClassFirst_(index);
		return res;
	}

	inline InstanceField* ClassType::getInstanceFieldBaseClassFirst(size_t index)
	{
		InstanceField* res = getInstanceFieldBaseClassFirst_(index);
		return res;
	}

	inline bool ClassType::hasDynamicInstanceField() const
	{
		return m_hasDynamicInstanceField;
	}


#}

}
