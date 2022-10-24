#import "Type.i"
#import "Enumerator.i"


namespace paf
{
#{
	class Variant;
	class InstanceProperty;
	class PrimitiveType;
#}

	abstract class(enumeration_type)#PAFCORE_EXPORT EnumType : Type
	{
		size_t _getEnumeratorCount_();
		Enumerator* _getEnumerator_(size_t index);
		Enumerator* _getEnumeratorByValue_(int value);
		Enumerator* _getEnumeratorByName_(string_t name);
#{
	public:
		EnumType(const char* name, const char* declarationFile);
	public:
		//override Type
		virtual bool destruct(void* address) override;
		virtual bool copyConstruct(void* dst, const void* src) override;
		virtual bool copyAssign(void* dst, const void* src) override;
		virtual Metadata* findMember(const char* name) override;
	public:
		void castToPrimitive(void* dst, PrimitiveType* dstType, const void* src);
		void castFromPrimitive(void* dst, PrimitiveType* srcType, const void* src);
		Enumerator* findEnumerator(const char* name);
	public:
		static paf::ErrorCode Enum_get__name_(paf::InstanceProperty* instanceProperty, paf::Variant* that, paf::Variant* value);
	public:
		Enumerator* m_enumerators;
		size_t m_enumeratorCount;
		InstanceProperty* m_instanceProperties;
		size_t m_instancePropertyCount;
#}
	};
}