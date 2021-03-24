#include "Timer.h"
#include "SystemTime.h"

Timer::Timer() :
	m_DeltaTime(-1),
	m_BaseTick(0),
	m_PausedTime(0),
	m_StopTick(0),
	m_PrevTick(0),
	m_CurrTick(0),
	m_Stopped(false)
{
	SystemTime::Initialize();
}

f64 Timer::GetTotalTime() const
{
	if (m_Stopped)
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// m_StopTick - m_BaseTick includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from m_StopTick:  
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime
		return SystemTime::TicksToSeconds(m_StopTick - m_BaseTick) - m_PausedTime;
	}
	else
	{
		// The distance mCurrTick - mBaseTick includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mCurrTime:  
		//
		//  (mCurrTick - mPausedTime) - mBaseTick
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mCurrTime
		return SystemTime::TicksToSeconds(m_CurrTick - m_BaseTick) - m_PausedTime;
	}
}

void Timer::Reset()
{
	m_StopTick = 0;
	m_Stopped = false;
	m_BaseTick = m_PrevTick = SystemTime::GetCurrentTick();
}

void Timer::Start()
{
	if (!m_Stopped)
		return;

	auto startTick = SystemTime::GetCurrentTick();

	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	m_PausedTime += SystemTime::TicksToSeconds(startTick - m_StopTick);
	m_PrevTick = startTick;
	m_StopTick = 0;
	m_Stopped = false;
}

void Timer::Stop()
{
	if (m_Stopped)
		return;

	m_StopTick = SystemTime::GetCurrentTick();
	m_Stopped = true;
}

void Timer::Tick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	m_CurrTick = SystemTime::GetCurrentTick();

	// Time difference between this frame and the previous.
	m_DeltaTime = SystemTime::TicksToSeconds(m_CurrTick - m_PrevTick);

	// Prepare for next frame.
	m_PrevTick = m_CurrTick;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (m_DeltaTime < 0.0)
		m_DeltaTime = 0.0;
}
