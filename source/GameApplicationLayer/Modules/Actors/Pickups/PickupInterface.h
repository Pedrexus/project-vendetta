#pragma once

#include "../ActorComponent.h"

class PickupInterface : public ActorComponent
{
public:
    virtual void Apply(std::weak_ptr<Actor> actor) = 0;
};
