#import "Reference.i"

namespace pafcore
{
	class(noncopyable) #PAFCORE_EXPORT Object : Reference
	{
#{
	public:
		virtual bool serializable() const
		{
			return true;
		}

		virtual bool editable() const
		{
			return true;
		}

		virtual bool propertySerializable(const char* propertyName) const
		{
			return true;
		}

		virtual bool propertyEditable(const char* propertyName) const
		{
			return true;
		}
#}
	};

#{
	class PAFCORE_EXPORT RefCounted
	{
	public:
		virtual long_t incRefCount() = 0;
		virtual long_t decRefCount() = 0;
		virtual size_t baseAddress() const = 0;
	};
#}

	class(noncopyable) #PAFCORE_EXPORT RefCountedObject : Object
	{
#{
	public:
		virtual long incRefCount() = 0;
		virtual long decRefCount() = 0;
		virtual size_t baseAddress() const = 0;
#}
	};
}