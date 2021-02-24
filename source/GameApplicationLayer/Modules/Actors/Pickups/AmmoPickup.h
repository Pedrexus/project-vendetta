#pragma once

#include <pch.h>

#include "../Actor.h"
#include "PickupInterface.h"

class AmmoPickup : public PickupInterface
{
public:
	inline bool Init(tinyxml2::XMLElement* node) override { return true; };
	virtual void Apply(std::weak_ptr<Actor> actor) override;

	static inline ActorComponentId GetName() { return "AmmoPickup"; }
};