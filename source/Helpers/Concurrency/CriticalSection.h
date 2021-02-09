#pragma once

#include <pch.h>

#include "Helpers/Classes/noncopyable.h"

class CriticalSection : public noncopyable
{

protected:
	// the critical section itself
	mutable CRITICAL_SECTION m_cs;

public:
	CriticalSection() { InitializeCriticalSection(&m_cs); }
	~CriticalSection() { DeleteCriticalSection(&m_cs); }

	void Lock() { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }
};