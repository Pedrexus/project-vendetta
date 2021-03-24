#pragma once

#include <pch.h>
#include <types.h>
#include <macros.h>


namespace SystemTime
{
    // The amount of time that elapses between ticks of the performance counter
    static f64 s_CpuTickDelta = 0;

    // Query the performance counter frequency
    inline void Initialize(void)
    {
        LARGE_INTEGER frequency;
        ASSERT(QueryPerformanceFrequency(&frequency));
        s_CpuTickDelta = 1.0 / static_cast<f64>(frequency.QuadPart);
    }

    // Query the current value of the performance counter
    inline u64 GetCurrentTick(void)
    {
        LARGE_INTEGER count;
        ASSERT(QueryPerformanceCounter(&count));
        return static_cast<u64>(count.QuadPart);
    }

    inline f64 TicksToSeconds(u64 TickCount)
    {
        return static_cast<f64>(TickCount) * s_CpuTickDelta;
    }
};