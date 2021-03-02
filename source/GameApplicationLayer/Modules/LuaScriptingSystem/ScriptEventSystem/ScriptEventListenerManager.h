#pragma once

#include <pch.h>
#include "ScriptEventListener.h"

//---------------------------------------------------------------------------------------------------------------------
// This class manages the C++ ScriptListener objects needed for script event listeners.
// Chapter 12, page 385
//---------------------------------------------------------------------------------------------------------------------
class ScriptEventListenerManager
{
	// typedef std::set<ScriptEventListener*> ScriptEventListenerSet;
	std::set<ScriptEventListener*> m_listenersSet;

public:
	~ScriptEventListenerManager(void);
	inline void AddListener(ScriptEventListener* pListener) { m_listenersSet.insert(pListener); }
	void DestroyListener(ScriptEventListener* pListener);
};