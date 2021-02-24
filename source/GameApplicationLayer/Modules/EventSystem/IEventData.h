#pragma once

#include <pch.h>
#include <types.h>

typedef char* EventType;

#ifdef _DEBUG
	typedef std::ostringstream EventSerializeStream;
	typedef std::istringstream EventDeserializeStream;
#else
	typedef std::ostream EventSerializeStream
	typedef std::istream EventDeserializeStream;
#endif // _DEBUG

//---------------------------------------------------------------------------------------------------------------------
// IEventData                               - Chapter 11, page 310
// Base type for event object hierarchy, may be used itself for simplest event notifications such as those that do 
// not carry additional payload data. If any event needs to propagate with payload data it must be defined separately.
//---------------------------------------------------------------------------------------------------------------------
class IEventData
{
public:
	virtual ~IEventData(void) {}
	virtual const EventType& GetEventType(void) const = 0;
	virtual f32 GetTimestamp(void) const = 0;
	virtual void Serialize(EventSerializeStream& out) const = 0;
	virtual void Deserialize(EventDeserializeStream& in) = 0;
	virtual std::shared_ptr<IEventData> Copy(void) const = 0;
	virtual const std::string GetName(void) const = 0;

	//GCC_MEMORY_WATCHER_DECLARATION();
};
