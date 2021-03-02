#pragma once

#include "../../EventSystem.h"

class ScriptEventListener
{
	EventListener::Id m_listenerId; // TODO: remove this dependency
	EventType m_eventType;
	LuaPlus::LuaObject m_scriptCallbackFunction;

public:
	inline explicit ScriptEventListener(const EventListener::Id id, const EventType& eventType, const LuaPlus::LuaObject& scriptCallbackFunction) :
		m_listenerId(id),
		m_eventType(eventType),
		m_scriptCallbackFunction(scriptCallbackFunction)
	{};
	inline ~ScriptEventListener(void)
	{
		if (auto pEventMgr = EventManager::Get())
			pEventMgr->RemoveListener(m_listenerId, m_eventType);
	};

	void ScriptEventDelegate(std::shared_ptr<IEventData> pEvent);
	inline EventListener::Delegate GetDelegate(void)
	{ 
		return [this] (std::shared_ptr<IEventData> e) { ScriptEventListener::ScriptEventDelegate(e); };
	}
	inline EventListener::Pair GetPair(void)
	{
		return std::make_pair(m_listenerId, GetDelegate());
	}
};