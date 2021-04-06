#include "ScriptsExports.h"

#include "ScriptEventSystem/ScriptEvent.h"
#include "ScriptEventSystem/ScriptEventListenerManager.h"

#include "../../GameApp.h"
#include <GameLogicLayer/Game.h>

class InternalScriptExports
{
	static ScriptEventListenerManager* s_pScriptEventListenerMgr;
	static std::shared_ptr<ScriptEvent> BuildEvent(EventType eventType, LuaPlus::LuaObject& eventData)
	{
		// create the event from the event type
		std::shared_ptr<ScriptEvent> pEvent{ ScriptEvent::CreateEventFromScript(eventType) };
		if (!pEvent)
			return nullptr;

		// set the event data that was passed in
		if (!pEvent->SetEventData(eventData))
			return nullptr;

		return pEvent;
	}

public:
	// initialization
	static bool Init(void)
	{
		if (!s_pScriptEventListenerMgr)
			s_pScriptEventListenerMgr = NEW ScriptEventListenerManager;
		return true;
	}
	static void Destroy(void);

	//---------------------------------------------------------------------------------------------------------------------
	// Loads a script resource then executes it.  
	// This is used by the require() function in script (defined in PreInit.lua).
	//---------------------------------------------------------------------------------------------------------------------
	static bool LoadAndExecuteScriptResource(const char* scriptResource)
	{
		auto game = Game::Get();
		if (game->GetResourceCache()->IsUsingDevelopmentDirectories())
		{
			// If we're using development directories, have Lua execute the file directly instead of going through 
			// the resource cache.  This allows Decoda to see the file for debugging purposes.
			auto path = std::string("..\\Assets\\") + scriptResource;
			LuaStateManager::Get()->ExecuteFile(path.c_str());
			return true;
		}
		else
		{
			Resource resource(scriptResource);
			auto pResourceHandle = game->GetResourceCache()->GetHandle(&resource);  // this actually loads the Lua file from the zip file
			return (bool) pResourceHandle;
		}
	}

	// actors
	// static int CreateActor(const char* actorArchetype, LuaPlus::LuaObject luaPosition, LuaPlus::LuaObject luaYawPitchRoll);

	// event system
	static u64 RegisterEventListener(EventListener::Id listenerId, EventType eventType, LuaPlus::LuaObject callbackFunction)
	{
		ASSERT(s_pScriptEventListenerMgr);

		if (callbackFunction.IsFunction())
		{
			// create the C++ listener proxy and set it to listen for the event
			ScriptEventListener* pListener = NEW ScriptEventListener(listenerId, eventType, callbackFunction);
			s_pScriptEventListenerMgr->AddListener(pListener);
			EventManager::Get()->AddListener(pListener->GetPair(), eventType);

			// convert the pointer to an unsigned long to use as the handle
			return reinterpret_cast<u64>(pListener);
		}

		LOG_ERROR("Attempting to register script event listener with invalid callback function");
		return 0;
	}
	static void RemoveEventListener(u64 listenerId)
	{
		ASSERT(s_pScriptEventListenerMgr);
		ASSERT(listenerId != 0);

		// convert the listenerId back into a pointer
		ScriptEventListener* pListener = reinterpret_cast<ScriptEventListener*>(listenerId);
		s_pScriptEventListenerMgr->DestroyListener(pListener);  // the destructor will remove the listener
	}
	static bool QueueEvent(EventType eventType, LuaPlus::LuaObject eventData)
	{
		if (auto pEvent = BuildEvent(eventType, eventData))
			return EventManager::Get()->QueueEvent(pEvent);
		return false;
	}
	static bool TriggerEvent(EventType eventType, LuaPlus::LuaObject eventData)
	{
		if (auto pEvent = BuildEvent(eventType, eventData))
			return EventManager::Get()->TriggerEvent(pEvent);
		return false;
	}

	// process system
	static void AttachScriptProcess(LuaPlus::LuaObject scriptProcess)
	{
		auto __object = scriptProcess.Lookup("__object");
		if (!__object.IsNil())
		{
			std::shared_ptr<Process> pProcess(static_cast<Process*>(__object.GetLightUserdata()));
			Game::Get()->AttachProcess(pProcess);
		}
		else
			LOG_ERROR("Couldn't find __object in script process");
	}

	// math
	// static float GetYRotationFromVector(LuaPlus::LuaObject vec3);
	// static float WrapPi(float wrapMe);
	// static LuaPlus::LuaObject GetVectorFromRotation(float angleRadians);

	// misc
	// static void LuaLog(LuaPlus::LuaObject text);
	// static unsigned long GetTickCount(void);

	// physics
	// static void ApplyForce(LuaPlus::LuaObject normalDir, float force, int actorId);
	// static void ApplyTorque(LuaPlus::LuaObject axis, float force, int actorId);
};
