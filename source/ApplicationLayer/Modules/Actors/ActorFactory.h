#pragma once

#include <ApplicationLayer/Actors.h>

// ActorFactory.h - Defines a factory for creating actors & components

// typedef UINT ActorId;

class ActorFactory
{
	ActorId m_lastActorId;

	inline ActorId GetNextActorId(void) { return ++m_lastActorId; }
		
protected:
	std::map<ActorComponentId, std::function<ActorComponent*()>> m_componentFactoryMap;
	
	// make sure you call the base-class version first.  If it returns NULL, you know it's not an engine component.
	virtual std::shared_ptr<ActorComponent> CreateComponent(tinyxml2::XMLElement* node);

public:
	ActorFactory(void);

	std::shared_ptr<Actor> CreateActor(const char* actorResource); // tinyxml2::XMLElement* overrides, const Mat4x4* initialTransform, const ActorId serversActorId);
	// void ModifyActor(shared_ptr<Actor> pActor, tinyxml2::XMLElement* overrides);
};