#import "Reference.i"
#import "NotifyHandler.i"
#import "DynamicObject.i"

#{
#include "Utility.h"
#}

namespace pafcore
{
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
		PAF_DECL_REFCOUNT_INTERFACE_DUMMY_IMPL
	public:
		NotifyHandler* m_first;
		NotifyHandler* m_second;
#}
	};

	class(value_object) #PAFCORE_EXPORT NotifyHandlerList
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

	class #PAFCORE_EXPORT PropertyChangedNotifySource : DynamicObject
	{
		void addNotifyHandler(PropertyChangedNotifyHandler* handler) const;
		void removeNotifyHandler(PropertyChangedNotifyHandler* handler) const;
		bool findNotifyHandler(PropertyChangedNotifyHandler* handler) const;
#{
	public:
		void notifyPropertyChanged(string_t propertyName, PropertyChangedFlag flag = PropertyChangedFlag::update, Iterator* iterator = 0);
		void nodifyPropertyAvailabilityChanged(string_t propertyName);
		void notifyDynamicPropertyChanged(string_t propertyName, PropertyChangedFlag flag = PropertyChangedFlag::update, Iterator* iterator = 0);
		void notifyUpdateDynamicProperty();
	protected:
		NotifyHandlerList m_notifyHandlerList;
#}
	};

#{
	inline void PropertyChangedNotifySource::addNotifyHandler(PropertyChangedNotifyHandler* handler) const
	{
		m_notifyHandlerList.addNotifyHandler(handler);
	}
	
	inline void PropertyChangedNotifySource::removeNotifyHandler(PropertyChangedNotifyHandler* handler) const	
	{
		m_notifyHandlerList.removeNotifyHandler(handler);
	}
	
	inline bool PropertyChangedNotifySource::findNotifyHandler(PropertyChangedNotifyHandler* handler) const
	{
		return m_notifyHandlerList.findNotifyHandler(handler);
	}
#}

}
