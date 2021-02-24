#pragma once

#include "IEventData.h"

class BaseEventData : public IEventData
{
	const float m_timestamp;

public:
	inline explicit BaseEventData(const f32 timestamp = 0.0f) : m_timestamp(timestamp) {}
	inline f32 GetTimestamp(void) const override { return m_timestamp; };

	// Serializing for network input / output
	inline void Serialize(EventSerializeStream& out) const override {}
	inline void Deserialize(EventDeserializeStream& in) override {}
};