#include "ScriptEvent.h"

#include "../LuaStateManager.h"

ScriptEvent::CreationFunctions ScriptEvent::s_creationFunctions;

LuaPlus::LuaObject ScriptEvent::GetEventData(void)
{
	if (!m_eventDataIsValid)
	{
		BuildEventData();
		m_eventDataIsValid = true;
	}

	return m_eventData;
}

bool ScriptEvent::SetEventData(LuaPlus::LuaObject eventData)
{
	m_eventData = eventData;
	m_eventDataIsValid = BuildEventFromScript();
	return m_eventDataIsValid;
}

void ScriptEvent::RegisterEventTypeWithScript(const char* key, EventType type) // key = event class name
{
	// get or create the EventType table
	LuaPlus::LuaObject eventTypeTable = LuaStateManager::Get()->GetGlobalVars().GetByName("EventType");

	if (eventTypeTable.IsNil())
		eventTypeTable = LuaStateManager::Get()->GetGlobalVars().CreateTable("EventType");

	// error checking
	ASSERT(eventTypeTable.IsTable());
	ASSERT(eventTypeTable[key].IsNil());

	// add the entry
	eventTypeTable.SetString(key, type);
}

void ScriptEvent::AddCreationFunction(EventType type, CreateEventForScriptFunctionType func)
{
	ASSERT(func);

	if (s_creationFunctions.find(type) != s_creationFunctions.end())
		LOG_ERROR("function was already registered");

	s_creationFunctions.insert(std::make_pair(type, func));
}

ScriptEvent* ScriptEvent::CreateEventFromScript(EventType type)
{
	auto findIt = s_creationFunctions.find(type);
	if (findIt != s_creationFunctions.end())
	{
		CreateEventForScriptFunctionType func = findIt->second;
		return func();
	}
	else
		LOG_ERROR("Couldn't find event type");
	return nullptr;
}
