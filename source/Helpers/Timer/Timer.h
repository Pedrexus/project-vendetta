#pragma once

#include <types.h>

class Timer
{
	f64 m_DeltaTime;
	u64 m_BaseTick;
	u64 m_PausedTime;
	u64 m_StopTick;
	u64 m_PrevTick;
	u64 m_CurrTick;
	bool m_Stopped;

public:
	Timer();

	f64 GetTotalTime() const; // in seconds
	inline milliseconds GetDeltaMilliseconds() const { return m_DeltaTime * 1000; }
	inline bool IsPaused() const { return m_Stopped; }

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.
};