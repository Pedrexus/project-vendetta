#pragma once

#include <ApplicationLayer/Actors.h>

typedef std::string ActorComponentId;

class ActorComponent
{
	friend class ActorFactory;

	inline void SetOwner(std::shared_ptr<Actor> owner) { m_owner = owner; }

protected:
	std::shared_ptr<Actor> m_owner; // link to the actor

public:
	inline virtual ~ActorComponent(void) { m_owner.reset(); } // QUESTION: does inline virtual make sense?

	// These functions are meant to be overridden by the implementation classes of the components.
	virtual bool Init(tinyxml2::XMLElement* node) = 0;
	virtual void PostInit(void) { } // default behavior = noop
	virtual void Update(int deltaMilisseconds) { }
	virtual void OnChanged(void) { }				// [mrmike] - added post-press

	// This function should be overridden by the interface class.
	static const ActorComponentId NAME;
};

