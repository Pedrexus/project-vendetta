#pragma once

#include <pch.h>

class CriticalSection 
{
	CriticalSection(const CriticalSection& x) = delete;
	CriticalSection& operator=(const CriticalSection& x) = delete;

protected:
	// the critical section itself
	mutable CRITICAL_SECTION m_cs;

public:
	CriticalSection() { InitializeCriticalSection(&m_cs); }
	~CriticalSection() { DeleteCriticalSection(&m_cs); }

	void Lock() { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }
};