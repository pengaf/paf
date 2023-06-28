#import "Type.i"

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
		virtual ErrorCode construct(void* address, Variant** args, uint32_t numArgs) override;
		virtual bool constructArray(void* address, size_t count) override;
		virtual bool destruct(void* self) override;
		virtual bool copyAssign(void* self, const void* src) override;
		virtual bool assign(void* self, Type* srcType, const void* src) override;
		virtual bool cast(Type* dstType, void* dst, const void* self) override;
		virtual Metadata* findMember(const char* name) override;
	public:
		//void castToPrimitive(void* dst, PrimitiveType* dstType, const void* src);
		//void castFromPrimitive(void* dst, PrimitiveType* srcType, const void* src);
		Enumerator* findEnumerator(const char* name);
	public:
		static paf::ErrorCode Enum_get__name_(InstanceProperty* instanceProperty, Variant* that, Variant* value);
	public:
		Enumerator* m_enumerators;
		size_t m_enumeratorCount;
		InstanceProperty* m_instanceProperties;
		size_t m_instancePropertyCount;
#}
	};

	abstract class(enumerator)#PAFCORE_EXPORT Enumerator : Metadata
	{
		EnumType* _type_ { get };
		int _value_ { get };
#{
	public:
		Enumerator(const char* name, Attributes* attributes, EnumType* type, int value);
	private:
		EnumType* m_type;
		int m_value;
#}
	};


#{
	inline EnumType* Enumerator::get__type_() const
	{
		return m_type;
	}

	inline int Enumerator::get__value_() const
	{
		return m_value;
	}
#}

}