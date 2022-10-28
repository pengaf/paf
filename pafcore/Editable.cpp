#include "Editable.h"
#include "Editable.mh"
#include "Editable.ic"
#include "Editable.mc"

BEGIN_PAF

void PropertyChangedNotifyHandler::onPropertyChanged(::paf::RawPtr<Introspectable> sender, string_t propertyName, PropertyChangedFlag flag, ::paf::RawPtr<Iterator> iterator)
{

}

void PropertyChangedNotifyHandler::onPropertyEditableChanged(::paf::RawPtr<Introspectable> sender, string_t propertyName)
{

}

//void PropertyChangedNotifyHandler::onDynamicPropertyChanged(::paf::RawPtr<Introspectable> sender, string_t propertyName, PropertyChangedFlag flag, ::paf::RawPtr<Iterator> iterator)
//{
//
//}
//
//void PropertyChangedNotifyHandler::onUpdateDynamicProperty(::paf::RawPtr<Introspectable> sender)
//{
//}


END_PAF
