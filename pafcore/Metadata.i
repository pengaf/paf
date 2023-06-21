#import "Utility.i"

namespace paf
{
#{
	struct Attribute
	{
		const char* name;
		const char* content;
	};
	struct Attributes
	{
		size_t count;
		Attribute* attributes;
	};
#}
	enum class MetaCategory
	{
		primitive,
		enumeration,
		object,
		string,//::paf::string_t ::paf::String, etc
		buffer,//::paf::buffer_t ::paf::Buffer, etc
		enumerator,
		instance_field,
		static_field,
		instance_property,
		static_property,
		instance_method,
		static_method,
		function_argument,
		function_result,
		primitive_type,
		enumeration_type,
		object_type,
		type_alias,
		name_space,
	};

	enum class Passing
	{
		value,
		reference,
		const_reference,
	};

	enum class TypeCompound
	{
		none,
		raw_ptr,
		raw_array,
		borrowed_ptr,
		borrowed_array,
		unique_ptr,
		unique_array,
		shared_ptr,
		shared_array,
	};

	enum class PropertyCategory
	{
		simple_property,
		array_property,
		collection_property,
	};

	abstract class #PAFCORE_EXPORT Metadata : Introspectable
	{
		string_t _name_ { get };
		MetaCategory _metaCategory_ { get };
		size_t _attributeCount_ { get };
		string_t _getAttributeName_(size_t index);
		string_t, int _getAttributeContent_(size_t index=#{0#});
		string_t _getAttributeContentByName_(string_t attributeName);
		bool _hasAttribute_(string_t attributeName);

#{
	public:
		Metadata(const char* name, Attributes* attributes = 0);
		//Metadata(const Metadata&) = default;
		//Metadata& operator=(const Metadata&) = default;
	public:
		bool operator < (const Metadata& arg) const;
	public:
		const char* m_name;
		Attributes* m_attributes;
#}
	};

#{

	inline size_t Metadata::get__attributeCount_() const
	{
		return m_attributes ? m_attributes->count : 0;
	}

	inline string_t Metadata::get__name_() const
	{
		return m_name;
	}

	class CompareMetaDataPtrByName
	{
	public:
		bool operator()(const Metadata* m1, const Metadata* m2);
	};

#}

}