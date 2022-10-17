#import "Utility.i"
#import "Introspectable.i"

namespace pafcore
{
#{
	const size_t max_method_param_count = 32;

	enum class ErrorCode
	{
		s_ok,
		e_invalid_namespace,
		e_name_conflict,
		e_void_variant,
		e_is_not_type,
		e_is_not_class,
		e_invalid_subscript_type,
		e_member_not_found,
		e_index_out_of_range,
		e_is_not_property,
		e_is_not_array_property,
		e_is_not_collection_property,
		e_property_get_not_implemented,
		e_property_set_not_implemented,
		e_property_size_not_implemented,
		e_property_iterate_not_implemented,
		e_field_is_an_array,
		e_invalid_type,
		e_invalid_object_type,
		e_invalid_field_type,
		e_invalid_property_type,
		e_invalid_too_few_arguments,
		e_invalid_too_many_arguments,
		e_invalid_this_type,
		e_invalid_arg_type_1,
		e_invalid_arg_type_2,
		e_invalid_arg_type_3,
		e_invalid_arg_type_4,
		e_invalid_arg_type_5,
		e_invalid_arg_type_6,
		e_invalid_arg_type_7,
		e_invalid_arg_type_8,
		e_invalid_arg_type_9,
		e_invalid_arg_type_10,
		e_invalid_arg_type_11,
		e_invalid_arg_type_12,
		e_invalid_arg_type_13,
		e_invalid_arg_type_14,
		e_invalid_arg_type_15,
		e_invalid_arg_type_16,
		e_invalid_arg_type_17,
		e_invalid_arg_type_18,
		e_invalid_arg_type_19,
		e_invalid_arg_type_20,
		e_invalid_arg_type_21,
		e_invalid_arg_type_22,
		e_invalid_arg_type_23,
		e_invalid_arg_type_24,
		e_invalid_arg_type_25,
		e_invalid_arg_type_26,
		e_invalid_arg_type_27,
		e_invalid_arg_type_28,
		e_invalid_arg_type_29,
		e_invalid_arg_type_30,
		e_invalid_arg_type_31,
		e_invalid_arg_type_32,
		e_not_implemented,
		e_script_error,
		e_script_dose_not_override,
	};

	extern const char* g_errorStrings[];

	//PAFCORE_EXPORT const char* ErrorCodeToString(ErrorCode errorCode);

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
		string,//spec object implement fromString & toString
		buffer,//spec object implement fromBuffer & toBuffer
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
		rvalue_reference,
		const_rvalue_reference,
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
		MetaCategory _category_ { get };
		size_t _attributeCount_ { get };
		string_t _getAttributeName_(size_t index);
		string_t _getAttributeContent_(size_t index);
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
		virtual long_t addRef();
		virtual long_t release();
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