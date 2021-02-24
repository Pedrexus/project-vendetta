#include "ActorFactory.h"

#include "Pickups/AmmoPickup.h"
#include "Pickups/HealthPickup.h"

#include "../ResourceCache.h"

#include <macros.h>
#include <helpers.h>


ActorFactory::ActorFactory(void) : m_lastActorId(0)
{
    m_componentFactoryMap[AmmoPickup::GetName()] = Factory::NewPointer<AmmoPickup>();
    m_componentFactoryMap[HealthPickup::GetName()] = Factory::NewPointer<HealthPickup>();
}

// TODO: change argument
std::shared_ptr<Actor> ActorFactory::CreateActor(const char* actorResource, tinyxml2::XMLElement* root)
{
    // Grab the root XML node
    // auto root = m_ResCache->GetData<ResourceData::XML>(actorResource)->GetRoot();
    if (!root)
    {
        LOG_ERROR("Failed to create actor from resource");
        return nullptr;
    }

    // create the actor instance
    std::shared_ptr<Actor> actor{ new Actor(GetNextActorId()) };
    if (!actor->Init(root))
    {
        LOG_ERROR("Failed to initialize actor");
        return nullptr;
    }

    bool initialTransformSet = false;

    // Loop through each child element and load the component
    for (auto node = root->FirstChildElement(); node; node = node->NextSiblingElement())
    {
        std::shared_ptr<ActorComponent> component = CreateComponent(node);
        if (component)
        {
            actor->AddComponent(component);
            component->SetOwner(actor);
        }
        else
        {
            // If an error occurs, we kill the actor and bail.  We could keep going, but the actor is will only be 
            // partially complete so it's not worth it.  Note that the `actor` instance will be destroyed because it
            // will fall out of scope with nothing else pointing to it.
            LOG_ERROR("Failed to add component to actor");
            return nullptr;
        }
    }

    // Now that the actor has been fully created, run the post init phase
    actor->PostInit();

    return actor;
}

// std::shared_ptr<ActorComponent> ActorFactory::CreateComponent(tinyxml2::XMLElement* p_XmlNode);
std::shared_ptr<ActorComponent> ActorFactory::CreateComponent(tinyxml2::XMLElement* node)
{
    const ActorComponentId name = node->Value();

    // TODO: encapsulate this logic
    std::shared_ptr<ActorComponent> component;
    auto findIterator = m_componentFactoryMap.find(name);
    if (findIterator != m_componentFactoryMap.end())
    {
        auto &componentFactory = findIterator->second;
        component.reset(componentFactory());
    }

    // initialize the component if we found one
    if (component)
    {
        if (!component->Init(node))
        {
            LOG_ERROR("Component failed to initialize " + name);
            return nullptr;
        }
    }
    else
    {
        LOG_ERROR("Couldn't find ActorComponent named " + name);
        return nullptr;  // fail
    }

    // component will be NULL if the component wasn't found.  
    // This isn't necessarily an error since you might have a 
    // custom CreateComponent() function in a sub class.
    return component;
}