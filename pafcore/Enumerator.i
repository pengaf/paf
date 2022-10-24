#import "EnumType.i"

namespace paf
{

	abstract class(enumerator)#PAFCORE_EXPORT Enumerator : Metadata
	{
		EnumType _type_ { get* };
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
	inline ::paf::RawPtr<EnumType> Enumerator::get__type_() const
	{
		return m_type;
	}

	inline int Enumerator::get__value_() const
	{
		return m_value;
	}
#}
}