#pragma once

#include <pch.h>
#include <macros.h>
#include <helpers.h>

class LuaStateManager
{
#pragma region Singleton
private:
    inline static LuaStateManager* instance;
    inline static std::mutex mutex;

protected:
    LuaStateManager() : m_pLuaState(nullptr), m_lastError("initial error") {};
    ~LuaStateManager() = default;

public:
    LuaStateManager(LuaStateManager& other) = delete; // Singletons should not be cloneable.
    void operator=(const LuaStateManager&) = delete; // Singletons should not be assignable.

    static inline void Destroy() { SAFE_DELETE(instance); };
    static inline LuaStateManager* Get()
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (instance == nullptr)
        {
            LOG("Lua", "LuaStateManager instantiated");
            instance = NEW LuaStateManager();
        }

        return instance;
    }
#pragma endregion

private:
    LuaPlus::LuaState* m_pLuaState;
    std::string m_lastError;

    void ThrowLastError();
    inline void ClearStack(void) { m_pLuaState->SetTop(0); }

public:
    // IScriptManager interface
    bool Init(void);
    inline void ExecuteFile(const char* path) { m_pLuaState->DoFile(path) != 0 ? ThrowLastError() : noop; };
    inline void ExecuteString(const char* str) { m_pLuaState->DoString(str) != 0 ? ThrowLastError() : noop; };

    inline LuaPlus::LuaObject GetGlobalVars(void) { return m_pLuaState->GetGlobals(); }
    inline LuaPlus::LuaState* GetLuaState(void) const { return m_pLuaState; };

    // public helpers
    LuaPlus::LuaObject CreatePath(const char* pathString, bool toIgnoreLastElement = false);
    // TODO: void ConvertVec3ToTable(const Vec3& vec, LuaPlus::LuaObject& outLuaTable) const;
    // TODO: void ConvertTableToVec3(const LuaPlus::LuaObject& luaTable, Vec3& outVec3) const;   
};