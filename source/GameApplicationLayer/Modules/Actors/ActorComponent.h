#pragma once

class Actor;

#include "Actor.h"

typedef std::string ActorComponentId;

class ActorComponent
{
	friend class ActorFactory;

	inline void SetOwner(std::shared_ptr<Actor> owner) { m_owner = owner; }

protected:
	std::shared_ptr<Actor> m_owner; // link to the actor

public:
	inline virtual ~ActorComponent(void) { m_owner.reset(); }

	// These functions are meant to be overridden by the implementation classes of the components.
	virtual bool Init(tinyxml2::XMLElement* node) = 0;
	virtual void PostInit(void) { } // default behavior = noop
	virtual void Update(milliseconds dt) { }
	virtual void OnChanged(void) { }				// [mrmike] - added post-press

	static inline ActorComponentId GetName() { return "?"; }
};

