#pragma once

#include <types.h>

#include "../../ProcessSystem.h"

// static constexpr auto Lua

class ScriptProcess : public Process
{
    u64 m_frequency, m_time;
    LuaPlus::LuaObject
        m_scriptInitFunction, 
        m_scriptUpdateFunction,
        m_scriptSuccessFunction, 
        m_scriptFailFunction, 
        m_scriptAbortFunction,
        m_self;

public:
    /*
        This exposes the ScriptProcess class to Lua.

        Without too much trouble, you should be able to create 
        a system that allows you to expose any arbitrary class 
        to Lua and enable Lua classes to inherit from them.
    */
    static void RegisterScriptClass(void);

protected:
    // Process interface
    void OnInit(void) override;
    void OnUpdate(milliseconds dt) override;
    void OnSuccess(void) override;
    void OnFail(void) override;
    void OnAbort(void) override;

private:
    // don't allow construction outside of this class
    ScriptProcess() : 
        m_frequency(0),
        m_time(0)
    {};

    // private helpers
    static LuaPlus::LuaObject CreateFromScript(LuaPlus::LuaObject self, LuaPlus::LuaObject constructionData, LuaPlus::LuaObject originalSubClass);
    virtual bool BuildCppDataFromScript(LuaPlus::LuaObject scriptClass, LuaPlus::LuaObject constructionData);

    // These are needed because the base-class version of these functions are all const and LuaPlus can't deal
    // with registering const functions.
    inline bool ScriptIsAlive(void) { return IsAlive(); }
    inline bool ScriptIsDead(void) { return IsDead(); }
    inline bool ScriptIsPaused(void) { return IsPaused(); }

    // This wrapper function is needed so we can translate a Lua script object to something C++ can use.
    void ScriptAttachChild(LuaPlus::LuaObject child);
};
