#import "ClassType.i"

#{
#include "Utility.h"
#}

namespace pafcore
{
#{
	class ClassType;
	class PAFCORE_EXPORT IntrospectableInterface : public Interface
	{
	public:
		virtual ClassType* getType() = 0;
	};
#}

	class(noncopyable) #PAFCORE_EXPORT Introspectable ## : public IntrospectableInterface
	{ 
		nocode bool isTypeOf(ClassType* classType);
		nocode bool isStrictTypeOf(ClassType* classType);
#{
	public:
		bool isTypeOf(ClassType* classType);

		template<typename T>
		bool isTypeOf()
		{
			return isTypeOf(T::GetType());
		}

		bool isStrictTypeOf(ClassType* classType)
		{
			return getType() == classType;
		}

		template<typename T>
		bool isStrictTypeOf()
		{
			return isStrictTypeOf(T::GetType());
		}

		void* castTo(ClassType* classType);

		template<typename T>
		T* castTo()
		{
			return reinterpret_cast<T*>(castTo(T::GetType()));
		}
#}
	};
}