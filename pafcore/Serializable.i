#import "Utility.i"

namespace paf
{
	class #PAFCORE_EXPORT Serializable : Introspectable
	{
#{
	public:
		//virtual bool serializable() const
		//{
		//	return true;
		//}

		virtual bool propertySerializable(const char* propertyName) const
		{
			return true;
		}

#}
	};
}