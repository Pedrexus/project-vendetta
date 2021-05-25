#pragma once

#include <dx12pch.h>
#include <Helpers/Settings/Settings.h>

class IRendered
{
    // Indicates object data has changed 
    // and we need to update the constant buffer
    u32 _NumFramesDirty = Settings::GetInt("graphics-frame-resources");

public:
    std::string Name;

    // void* buffer;
    // std::unordered_map<string, u64> indices;

    bool IsDirty() const
    {
        return _NumFramesDirty > 0;
    }
    void Clean()
    {
        _NumFramesDirty--;
    }
    void Reset()
    {
        _NumFramesDirty = Settings::GetInt("graphics-frame-resources");
    }
};