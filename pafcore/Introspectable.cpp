#include "Introspectable.h"
#include "Introspectable.mh"
#include "Introspectable.ic"
#include "Introspectable.mc"

BEGIN_PAFCORE

bool Introspectable::isTypeOf(ClassType* classType)
{
	ClassType* thisType = getType();
	return thisType->isType(classType);
}

void* Introspectable::castTo(ClassType* classType)
{
	size_t offset;
	ClassType* thisType = getType();
	if (thisType->getClassOffset(offset, classType))
	{
		void* address = getAddress();
		return (void*)((size_t)address + offset);
	}
	return 0;
}

END_PAFCORE
