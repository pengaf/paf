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
		update, //����ֵ�����������������ԣ��� iterator ָ������, ����Ϊnull
		remove, //���������Ƴ�һ��֮ǰ��iterator ָ�򼴽��Ƴ�����
		add, //������������һ��֮��iterator ָ������ӵ���
		reset,//�������Զ����任��iterator Ϊ null
	};

	override class #PAFCORE_EXPORT NotifyHandler : Introspectable
	{
	};

}
