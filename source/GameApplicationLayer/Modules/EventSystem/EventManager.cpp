#include "EventManager.h"

#include <const.h>

EventManager* EventManager::GetInstance()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (instance == nullptr)
	{
		LOG("Events", "EventManager instantiated");
		instance = NEW EventManager();
	}

	return instance;
}

bool EventManager::AddListener(const EventListener::Pair& eventPair, const EventType& type)
{
	LOG("Events", "Adding delegate function for event type: " + std::string(type));

	auto& eventListeners = m_eventListenersMap[type]; // this will find or create the entry

	if (eventListeners.contains(eventPair.first))
	{
		LOG_WARNING("Attempting to double-register delegate " + eventPair.first);
		return false;
	}

	eventListeners[eventPair.first] = eventPair.second;
	LOG("Events", "Successfully added delegate for event type: " + std::string(type));

	return true;
}

bool EventManager::RemoveListener(const EventListener::Id id, const EventType& type)
{
	LOG("Events", "Removing " + id + " from event type: " + std::string(type));

	auto findIt = m_eventListenersMap.find(type);
	if (findIt != m_eventListenersMap.end())
	{
		auto& listeners = findIt->second;
		const auto& numErased = listeners.erase(id);

		if (numErased > 0)
		{
			LOG("Events", "Successfully removed " + std::to_string(numErased) + " " + id + " from event type: " + std::string(type));
			return true;
		}
		else
			LOG_WARNING("Attempting to remove inexistent listener " + id + " from event type: " + std::string(type));
	}

	return false;
}

bool EventManager::TriggerEvent(const std::shared_ptr<IEventData>& pEvent) const
{
	LOG("Events", "Attempting to trigger event " + std::string(pEvent->GetName()));
	bool processed = false;

	auto findIt = m_eventListenersMap.find(pEvent->GetEventType());
	if (findIt != m_eventListenersMap.end())
	{
		const auto& listeners = findIt->second;
		std::for_each(
			LOOP_EXECUTION_POLICY,
			listeners.begin(),
			listeners.end(),
			[pEvent, &processed] (const EventListener::Pair& eventPair)
			{ 
				EventListener::Delegate f = eventPair.second;
				
				LOG("Events", "Sending Event " + std::string(pEvent->GetName()) + " to delegate.");

				f(pEvent);
				
				processed = true;
			}
		);
	}

	return processed;
}

bool EventManager::QueueEvent(const std::shared_ptr<IEventData>& pEvent)
{
	// make sure the event is valid
	if (!pEvent)
	{
		LOG_ERROR("Invalid event in EventManager::QueueEvent() call");
		return false;
	}

	LOG("Events", "Attempting to queue event: " + pEvent->GetName());

	auto findIt = m_eventListenersMap.find(pEvent->GetEventType());
	if (findIt != m_eventListenersMap.end())
	{
		m_queues[m_activeQueue].push_back(pEvent);
		LOG("Events", "Successfully queued event: " + pEvent->GetName());
		return true;
	}
	else
	{
		LOG("Events", "Skipping event since there are no delegates registered to receive it: " + pEvent->GetName());
		return false;
	}
}

bool EventManager::ThreadSafeQueueEvent(const std::shared_ptr<IEventData>& pEvent)
{
	m_realtimeEventQueue.push(pEvent);
	return true;
}

bool EventManager::AbortEvent(const EventType& type, bool abortAllOfType)
{
	auto findIt = m_eventListenersMap.find(type);
	if (findIt != m_eventListenersMap.end())
	{
		EventQueue& eventQueue = m_queues[m_activeQueue];
		auto predicate = [type] (std::shared_ptr<IEventData> e) -> bool { return e->GetEventType() == type; };

		if (abortAllOfType)
		{
			eventQueue.remove_if(predicate);
			return true;
		}
		else
		{
			auto it = std::find_if(eventQueue.begin(), eventQueue.end(), predicate);
			if (it != eventQueue.end())
			{
				eventQueue.erase(it);
				return true;
			}
		}
	}

	return false;
}

bool EventManager::Update(milliseconds timeout)
{
	u64 currentTime = GetTickCount64();
	u64 maxTime = (timeout == INFINITE_TIME) ? INFINITE_TIME : (currentTime + timeout);

	// This section added to handle events from other threads.  Check out Chapter 20.
	std::shared_ptr<IEventData> pRealtimeEvent;
	while (m_realtimeEventQueue.try_pop(pRealtimeEvent))
	{
		QueueEvent(pRealtimeEvent);

		currentTime = GetTickCount64();
		if (maxTime != INFINITE_TIME && currentTime >= maxTime)
			LOG_ERROR("A realtime process is spamming the event manager!");
	}

	// swap active queues and clear the new queue after the swap
	u8 queueToProcess = m_activeQueue;
	m_activeQueue = (m_activeQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_queues[m_activeQueue].clear();

	LOG("Events", "Processing Event Queue: " + std::to_string(m_queues[queueToProcess].size()) + " events to process");

	// Process the queue
	while (!m_queues[queueToProcess].empty())
	{
		// pop the front of the queue
		auto& pEvent = m_queues[queueToProcess].front();
		m_queues[queueToProcess].pop_front();

		LOG("Events", "\tProcessing Event " + pEvent->GetName());

		const auto& eventType = pEvent->GetEventType();

		// find all the delegate functions registered for this event
		auto findIt = m_eventListenersMap.find(eventType);
		if (findIt != m_eventListenersMap.end())
		{
			const auto& eventListeners = findIt->second;
			LOG("Events", "\tFound " + std::to_string(eventListeners.size()) + " delegates");

			// call each listener
			for (auto& [id, delegate]: eventListeners)
			{
				LOG("Events", "\t\tSending event " + pEvent->GetName() + " to delegate");
				delegate(pEvent);
			}
		}

		// check to see if time ran out
		currentTime = GetTickCount64();
		if (maxTime != INFINITE_TIME && currentTime >= maxTime)
		{
			LOG("Events", "Aborting event processing; time ran out");
			break;
		}
	}

	// If we couldn't process all of the events, push the remaining events to the new active queue.
	// Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
	bool queueIsFlushed = m_queues[queueToProcess].empty();
	if (!queueIsFlushed)
	{
		while (!m_queues[queueToProcess].empty())
		{
			auto& pEvent = m_queues[queueToProcess].back();
			m_queues[queueToProcess].pop_back();
			m_queues[m_activeQueue].push_front(pEvent);
		}
	}

	return queueIsFlushed;
}
