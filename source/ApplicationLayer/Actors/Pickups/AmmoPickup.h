#pragma once

#include <ApplicationLayer/Actors/Pickups/PickupInterface.h>

class AmmoPickup : public PickupInterface
{
public:
    virtual bool Init(tinyxml2::XMLElement* node) override;
    virtual void Apply(std::weak_ptr<Actor> actor) override;
};