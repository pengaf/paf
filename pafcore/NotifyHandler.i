#import "Utility.i"

#{
#include "Utility.h"
#}

namespace paf
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


	class #PAFCORE_EXPORT NotifyHandlerLink : NotifyHandler
	{
#{
	public:
		static NotifyHandlerLink* New(NotifyHandler* first, NotifyHandler* second);
		static void AddToList(NotifyHandler*& root, NotifyHandler* p);
		static void RemoveFromList(NotifyHandler*& root, NotifyHandler* p);
		static bool FindInList(NotifyHandler* root, NotifyHandler* p);
	public:
		template<typename Handler, typename Sender, typename Func = void(Handler::*)(Sender*)>
		static void TransmitNotify(NotifyHandler* handler, Sender* sender, Func func)
		{
			PAF_ASSERT(0 != handler);
			if (handler->isStrictTypeOf<NotifyHandlerLink>())
			{
				TransmitNotify<Handler, Sender, Func>(static_cast<NotifyHandlerLink*>(handler)->m_first, sender, func);
				TransmitNotify<Handler, Sender, Func>(static_cast<NotifyHandlerLink*>(handler)->m_second, sender, func);
			}
			else
			{
				(static_cast<Handler*>(handler)->*func)(sender);
			}
		}
		template<typename Handler, typename Sender, typename A1, typename Func = void(Handler::*)(Sender*, A1)>
		static void TransmitNotify(NotifyHandler* handler, Sender* sender, A1 a1, Func func)
		{
			PAF_ASSERT(0 != handler);
			if (handler->isStrictTypeOf<NotifyHandlerLink>())
			{
				TransmitNotify<Handler, Sender, A1, Func>(static_cast<NotifyHandlerLink*>(handler)->m_first, sender, a1, func);
				TransmitNotify<Handler, Sender, A1, Func>(static_cast<NotifyHandlerLink*>(handler)->m_second, sender, a1, func);
			}
			else
			{
				(static_cast<Handler*>(handler)->*func)(sender, a1);
			}
		}
		template<typename Handler, typename Sender, typename A1, typename A2, typename Func = void(Handler::*)(Sender*, A1, A2)>
		static void TransmitNotify(NotifyHandler* handler, Sender* sender, A1 a1, A2 a2, Func func)
		{
			PAF_ASSERT(0 != handler);
			if (handler->isStrictTypeOf<NotifyHandlerLink>())
			{
				TransmitNotify<Handler, Sender, A1, A2, Func>(static_cast<NotifyHandlerLink*>(handler)->m_first, sender, a1, a2, func);
				TransmitNotify<Handler, Sender, A1, A2, Func>(static_cast<NotifyHandlerLink*>(handler)->m_second, sender, a1, a2, func);
			}
			else
			{
				(static_cast<Handler*>(handler)->*func)(sender, a1, a2);
			}
		}
		template<typename Handler, typename Sender, typename A1, typename A2, typename A3, typename Func = void(Handler::*)(Sender*, A1, A2, A3)>
		static void TransmitNotify(NotifyHandler* handler, Sender* sender, A1 a1, A2 a2, A3 a3, Func func)
		{
			PAF_ASSERT(0 != handler);
			if (handler->isStrictTypeOf<NotifyHandlerLink>())
			{
				TransmitNotify<Handler, Sender, A1, A2, A3, Func>(static_cast<NotifyHandlerLink*>(handler)->m_first, sender, a1, a2, a3, func);
				TransmitNotify<Handler, Sender, A1, A2, A3, Func>(static_cast<NotifyHandlerLink*>(handler)->m_second, sender, a1, a2, a3, func);
			}
			else
			{
				(static_cast<Handler*>(handler)->*func)(sender, a1, a2, a3);
			}
		}
	public:
		NotifyHandlerLink(NotifyHandler* first, NotifyHandler* second) :
			m_first(first),
			m_second(second)
		{
			PAF_ASSERT(0 != first && 0 != second);
			PAF_ASSERT(!first->isStrictTypeOf<NotifyHandlerLink>() || !second->isStrictTypeOf<NotifyHandlerLink>());
		}
		~NotifyHandlerLink();

	public:
		bool find(NotifyHandler* p);
		NotifyHandler* remove(NotifyHandler* p);
	public:
		NotifyHandler* m_first;
		NotifyHandler* m_second;
#}
	};

	class #PAFCORE_EXPORT NotifyHandlerList
	{
		void addNotifyHandler(NotifyHandler* handler) const;
		void removeNotifyHandler(NotifyHandler* handler) const;
		bool findNotifyHandler(NotifyHandler* handler) const;
#{
	public:
		NotifyHandlerList();
		~NotifyHandlerList();
	public:
		mutable NotifyHandler* m_notifyHandler;
#}
	};


}
