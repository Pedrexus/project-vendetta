#include <pch.h>

#include "ScriptEventListener.h"
#include "ScriptEvent.h"

void ScriptEventListener::ScriptEventDelegate(std::shared_ptr<IEventData> pEvent)
{
	ASSERT(m_scriptCallbackFunction.IsFunction());  // this should never happen since it's validated before even creating this object

	// call the Lua function
	std::shared_ptr<ScriptEvent> pScriptEvent = std::static_pointer_cast<ScriptEvent>(pEvent);
	LuaPlus::LuaFunction<void*> Callback = m_scriptCallbackFunction;

	Callback(pScriptEvent->GetEventData());
}
