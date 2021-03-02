#include "ScriptProcess.h"
#include "../LuaStateManager.h"

static constexpr auto SCRIPT_PROCESS_NAME = "ScriptProcess";

void ScriptProcess::RegisterScriptClass(void)
{
	LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().CreateTable(SCRIPT_PROCESS_NAME);

	/*
		// .lua equivalent to:

		ScriptProcess = {
			__index: ScriptProcess,
			base: ScriptProcess, // sets metatable
			cpp: true,
			..., // C++ methods
			Create: CreateFromScript
		}
	*/
	metaTableObj.SetObject("__index", metaTableObj);
	metaTableObj.SetObject("base", metaTableObj);  // base refers to the parent class; ie the metatable
	metaTableObj.SetBoolean("cpp", true);

	// Register Script Class Functions
	static constexpr auto nullProcess = (Process*)nullptr;
	metaTableObj.RegisterObjectDirect("Succeed", nullProcess, &Process::Succeed);
	metaTableObj.RegisterObjectDirect("Fail", nullProcess, &Process::Fail);
	metaTableObj.RegisterObjectDirect("Pause", nullProcess, &Process::Pause);
	metaTableObj.RegisterObjectDirect("Unpause", nullProcess, &Process::Unpause);

	static constexpr auto nullScriptProcess = (ScriptProcess*)nullptr;
	metaTableObj.RegisterObjectDirect("IsAlive", nullScriptProcess, &ScriptProcess::ScriptIsAlive);
	metaTableObj.RegisterObjectDirect("IsDead", nullScriptProcess, &ScriptProcess::ScriptIsDead);
	metaTableObj.RegisterObjectDirect("IsPaused", nullScriptProcess, &ScriptProcess::ScriptIsPaused);
	metaTableObj.RegisterObjectDirect("AttachChild", nullScriptProcess, &ScriptProcess::ScriptAttachChild);

	metaTableObj.RegisterDirect("Create", &ScriptProcess::CreateFromScript);

}

void ScriptProcess::OnInit(void)
{
	Process::OnInit();

	if (!m_scriptInitFunction.IsNil())
	{
		LuaPlus::LuaFunction<void*> InitFunc { m_scriptInitFunction };
		InitFunc(m_self);
	}

	// No update function so bail immediately.  We may want to consider calling Succeed() here so that the child 
	// process is attached normally, but right now I'm assuming that the reason no OnUpdate() function was found
	// is due to a bug on the script side.  That means the child process may be dependent on this one.
	if (!m_scriptUpdateFunction.IsFunction())
		Fail();
}

void ScriptProcess::OnUpdate(milliseconds dt)
{
	m_time += dt;
	if (m_time >= m_frequency)
	{
		LuaPlus::LuaFunction<void*> UpdateFunc{ m_scriptUpdateFunction };
		UpdateFunc(m_self, m_time);
		m_time = 0;
	}
}

void ScriptProcess::OnSuccess(void)
{
	if (!m_scriptSuccessFunction.IsNil())
	{
		LuaPlus::LuaFunction<void*> SuccessFunc(m_scriptSuccessFunction);
		SuccessFunc(m_self);
	}
}

void ScriptProcess::OnFail(void)
{
	if (!m_scriptFailFunction.IsNil())
	{
		LuaPlus::LuaFunction<void*> FailFunc(m_scriptFailFunction);
		FailFunc(m_self);
	}
}

void ScriptProcess::OnAbort(void)
{
	if (!m_scriptAbortFunction.IsNil())
	{
		LuaPlus::LuaFunction<void*> AbortFunc(m_scriptAbortFunction);
		AbortFunc(m_self);
	}
}

void ScriptProcess::ScriptAttachChild(LuaPlus::LuaObject child)
{
	if (child.IsTable())
	{
		LuaPlus::LuaObject obj = child.GetByName("__object");
		if (!obj.IsNil())
		{
			// Casting a raw ptr to a smart ptr is generally bad, but Lua has no concept of what a shared_ptr 
			// is.  There's no easy way around it.
			std::shared_ptr<Process> pProcess(static_cast<Process*>(obj.GetLightUserdata()));

			if (pProcess)
				AttachChild(pProcess);
			else
				LOG_ERROR("invalid __object lightuserdata");
		}
		else
			LOG_ERROR("Attempting to attach child to ScriptProcess with no valid __object");
	}
	else
		LOG_ERROR("Invalid object type passed into ScriptProcess::ScriptAttachChild(); type = " + std::string(child.TypeName()));
}

LuaPlus::LuaObject ScriptProcess::CreateFromScript(LuaPlus::LuaObject self, LuaPlus::LuaObject constructionData, LuaPlus::LuaObject originalSubClass)
{
	// Note: The self parameter is not use in this function, but it allows us to be consistent when calling
	// Create().  The Lua version of this function needs self.
	LOG("Lua", std::string("Creating instance of ") + SCRIPT_PROCESS_NAME);
	ScriptProcess* pObj = NEW ScriptProcess;

	pObj->m_self.AssignNewTable(LuaStateManager::Get()->GetLuaState());
	if (pObj->BuildCppDataFromScript(originalSubClass, constructionData))
	{
		LuaPlus::LuaObject metaTableObj = LuaStateManager::Get()->GetGlobalVars().Lookup(SCRIPT_PROCESS_NAME);

		if (metaTableObj.IsNil())
			LOG_ERROR("ScriptProcess metatable is Nil");

		pObj->m_self.SetLightUserdata("__object", pObj);
		pObj->m_self.SetMetatable(metaTableObj);
	}
	else
	{
		// if building fails, this obj will be destroyed on both
		pObj->m_self.AssignNil(LuaStateManager::Get()->GetLuaState());
		SAFE_DELETE(pObj);
	}

	return pObj->m_self;
}

bool ScriptProcess::BuildCppDataFromScript(LuaPlus::LuaObject scriptClass, LuaPlus::LuaObject constructionData)
{
	auto GetByNameOrThrow = [scriptClass] (const std::string methodName, bool required = false)
	{
		auto temp = scriptClass.GetByName(methodName.c_str());
		if (!temp.IsFunction() && required)
			LOG_ERROR("No " + methodName + " found in script process; type == " + temp.TypeName());
		return temp;
	};

	if (scriptClass.IsTable())
	{
		m_scriptInitFunction = GetByNameOrThrow("OnInit");
		m_scriptSuccessFunction = GetByNameOrThrow("OnSuccess");
		m_scriptFailFunction = GetByNameOrThrow("OnFail");
		m_scriptSuccessFunction = GetByNameOrThrow("OnAbort");
		m_scriptUpdateFunction = GetByNameOrThrow("OnUpdate", true);
		if (!m_scriptUpdateFunction.IsFunction())
			return false;  // we're hosed, though I think returning false here just crashes the game :-/
	}
	else
	{
		LOG_ERROR("scriptClass is not a table in ScriptProcess::BuildCppDataFromScript()");
		return false;
	}

	if (constructionData.IsTable())
	{

		for (LuaPlus::LuaTableIterator constructionDataIt(constructionData); constructionDataIt; constructionDataIt.Next())
		{
			const auto key = constructionDataIt.GetKey().GetString();
			LuaPlus::LuaObject val = constructionDataIt.GetValue();

			if (strcmp(key, "frequency") == 0 && val.IsInteger())
				m_frequency = val.GetInteger();
			else
				m_self.SetObject(key, val);
		}
	}

	return true;
}
