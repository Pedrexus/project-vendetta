#pragma once

#include "IEventData.h"

#include <macros.h>

constexpr auto EVENTMANAGER_NUM_QUEUES = 2;

namespace EventListener
{
	typedef std::string Id;
	typedef std::function<void(const std::shared_ptr<IEventData>)> Delegate;
	typedef std::pair<Id, Delegate> Pair;
	typedef std::map<Id, Delegate> Listeners;
	typedef std::map<EventType, Listeners> Map;
}

// a game event is something that has happened in the most
// recent frame, such as an actor has been destroyed or moved.A process is something
// that takes more than one frame to process, such as an animation or monitoring a
// sound effect.
class EventManager
{

	typedef std::list<std::shared_ptr<IEventData>> EventQueue;

	EventListener::Map m_eventListenersMap;
	EventQueue m_queues[EVENTMANAGER_NUM_QUEUES];  // Without two queues, the program would hang in an infinite loop
	u8 m_activeQueue;  // index of actively processing queue; events enque to the opposing queue

	Concurrency::concurrent_queue<std::shared_ptr<IEventData>> m_realtimeEventQueue;

	inline static EventManager* instance;
	inline static std::mutex mutex;

protected:
	inline EventManager() : m_activeQueue(0) {};
	~EventManager() = default;

public:
	EventManager(EventManager& other) = delete; // Singletons should not be cloneable.
	void operator=(const EventManager&) = delete; // Singletons should not be assignable.

	static EventManager* GetInstance();
	inline EventManager* Destroy() { SAFE_DELETE(instance); };

	bool AddListener(const EventListener::Pair& eventPair, const EventType& type);
	bool RemoveListener(const EventListener::Id id, const EventType& type); // Returns false if the id was not found.

	// Fire off event NOW. Bypasses the queue entirely.
	bool TriggerEvent(const std::shared_ptr<IEventData>& pEvent) const;

	// Fire off event. Call the delegate function on the next call to Tick(), assuming there's enough time.
	bool QueueEvent(const std::shared_ptr<IEventData>& pEvent);
	bool ThreadSafeQueueEvent(const std::shared_ptr<IEventData>& pEvent);

	// Find the next-available instance of the named event type and remove it from the processing queue.  This 
	// may be done up to the point that it is actively being processed ...  e.g.: is safe to happen during event
	// processing itself.
	//
	// if allOfType is true, then all events of that type are cleared from the input queue.
	//
	// returns true if the event was found and removed, false otherwise
	bool AbortEvent(const EventType& type, bool abortAllOfType = false);

	// Allow for processing of any queued messages, optionally specify a processing time limit so that the event 
	// processing does not take too long. Note the danger of using this artificial limiter is that all messages 
	// may not in fact get processed.
	//
	// returns true if all messages ready for processing were completed, false otherwise (e.g. timeout )
	virtual bool Update(milliseconds timeout = INFINITE_TIME);

	// Getter for the main global event manager.  This is the event manager that is used by the majority of the 
	// engine, though you are free to define your own as long as you instantiate it with setAsGlobal set to false.
	// It is not valid to have more than one global event manager.
	// static IEventManager* Get(void);
};