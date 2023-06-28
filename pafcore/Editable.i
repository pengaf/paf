#import "Serializable.i"
#import "NotifyHandler.i"

namespace paf
{
	class #PAFCORE_EXPORT Editable : Serializable
	{
#{
	public:
		//virtual bool editable() const
		//{
		//	return true;
		//}

		virtual bool propertyEditable(const char* propertyName) const
		{
			return true;
		}

#}
	};

	//override class #PAFCORE_EXPORT PropertyChangedNotifyHandler : NotifyHandler
	//{
	//	override virtual void onPropertyChanged(Introspectable* sender, string_t propertyName, PropertyChangedFlag flag, Iterator* iterator);
	//	override virtual void onPropertyEditableChanged(Introspectable* sender, string_t propertyName);
	//};

}