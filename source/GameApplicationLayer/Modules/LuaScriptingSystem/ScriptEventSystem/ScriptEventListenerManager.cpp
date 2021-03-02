#include "ScriptEventListenerManager.h"

ScriptEventListenerManager::~ScriptEventListenerManager(void)
{
	for (auto pListener : m_listenersSet)
		delete pListener;
	m_listenersSet.clear();
}

void ScriptEventListenerManager::DestroyListener(ScriptEventListener* pListener)
{
	auto findIt = m_listenersSet.find(pListener);
	if (findIt != m_listenersSet.end())
	{
		m_listenersSet.erase(findIt);
		delete pListener;
	}
	else
	{
		LOG_ERROR("Couldn't find script listener in set; this will probably cause a memory leak");
	}
}
