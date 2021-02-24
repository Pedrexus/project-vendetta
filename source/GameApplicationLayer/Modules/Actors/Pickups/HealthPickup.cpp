#include "HealthPickup.h"

void HealthPickup::Apply(std::weak_ptr<Actor> actor)
{
	auto pStrongActor = std::shared_ptr<Actor>(actor);
	if (pStrongActor)
		LOG("Actor", "Applying health pickup to actor id " + pStrongActor->GetId());
}
