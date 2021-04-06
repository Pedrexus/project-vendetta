#include "LuaStateManager.h"

#include <Helpers/Functions.h>
#include <Helpers/String/String.h>

void LuaStateManager::ThrowLastError()
{
    // Note: If we get an error, we're hosed because LuaPlus throws an exception. So if this function
    // is called and the error at the bottom triggers, you might as well pack it in.

    auto stackObj = LuaPlus::LuaStackObject(m_pLuaState, -1);
    if (auto errStr = stackObj.GetString())
    {
        m_lastError = errStr;
        ClearStack();
    }
    else
        m_lastError = "Unknown Lua parse error";

    LOG_ERROR(m_lastError);
}

bool LuaStateManager::Init(void)
{
    m_pLuaState = LuaPlus::LuaState::Create(true);
    if (!m_pLuaState)
        return false;

    // register functions
    m_pLuaState->GetGlobals().RegisterDirect("ExecuteFile", (*this), &LuaStateManager::ExecuteFile);
    m_pLuaState->GetGlobals().RegisterDirect("ExecuteString", (*this), &LuaStateManager::ExecuteString);

    return true;
}

LuaPlus::LuaObject LuaStateManager::CreatePath(const char* pathString, bool toIgnoreLastElement)
{
    auto splitPath = String(pathString).Split('.');
    if (toIgnoreLastElement)
        splitPath.pop_back();

    auto context = GetGlobalVars();
    for (const auto& element : splitPath)
    {
        // make sure we still have a valid context
        if (context.IsNil())
        {
            LOG_ERROR("Something broke in CreatePath(); bailing out (element == " + element + ")");
            return context;  // this will be nil
        }

        // grab whatever exists for this element
        auto curr = context.GetByName(element.c_str());
        if (!curr.IsTable())
        {
            // if the element is not a table and not nil, we clobber it
            if (!curr.IsNil())
            {
                LOG_WARNING("Overwriting element '" + element + "' in table");
                context.SetNil(element.c_str());
            }

            // element is either nil or was clobbered so add the new table
            context.CreateTable(element.c_str());
        }

        context = context.GetByName(element.c_str());
    }

    // if we get here, we have created the path
    return context;
}
