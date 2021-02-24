#include "AmmoPickup.h"

void AmmoPickup::Apply(std::weak_ptr<Actor> actor)
{
    auto pStrongActor = std::shared_ptr<Actor>(actor);
    if (pStrongActor)
        LOG("Actor", "Applying ammo pickup to actor id " + pStrongActor->GetId());
}
