#import "Introspectable.i"
#import "NotifyHandlerList.i"
#import "Iterator.i"

#{
#include "Utility.h"
#}

namespace pafcore
{
	enum class PropertyChangedFlag
	{
		update, //属性值变更，如果是容器属性，则 iterator 指向变更项, 否则为null
		remove, //容器属性移除一项之前，iterator 指向即将移除的项
		add, //容器属性增加一项之后，iterator 指向刚增加的项
		reset,//容器属性多个项变换，iterator 为 null
	};

	override class #PAFCORE_EXPORT NotifyHandler : Introspectable
	{
	};

}
