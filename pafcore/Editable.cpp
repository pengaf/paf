#include "Editable.h"
#include "Editable.mh"
#include "Editable.ic"
#include "Editable.mc"

BEGIN_PAFCORE

void PropertyChangedNotifyHandler::onPropertyChanged(::pafcore::RawPtr<Introspectable> sender, string_t propertyName, PropertyChangedFlag flag, ::pafcore::RawPtr<Iterator> iterator)
{

}

void PropertyChangedNotifyHandler::onPropertyEditableChanged(::pafcore::RawPtr<Introspectable> sender, string_t propertyName)
{

}

//void PropertyChangedNotifyHandler::onDynamicPropertyChanged(::pafcore::RawPtr<Introspectable> sender, string_t propertyName, PropertyChangedFlag flag, ::pafcore::RawPtr<Iterator> iterator)
//{
//
//}
//
//void PropertyChangedNotifyHandler::onUpdateDynamicProperty(::pafcore::RawPtr<Introspectable> sender)
//{
//}


END_PAFCORE
