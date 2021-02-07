#include <ApplicationLayer/Actors/ActorFactory.h>

ActorFactory::ActorFactory(void)
{
    m_componentFactoryMap.emplace(AmmoPickup::NAME, [] { new AmmoPickup(); });
    m_componentFactoryMap.emplace(HealthPickup::NAME, [] { new HealthPickup(); });
}


std::shared_ptr<Actor> ActorFactory::CreateActor(const char* actorResource)
{
    // Grab the root XML node
    tinyxml2::XMLElement* root = XmlResourceLoader::LoadAndReturnRootXmlElement(actorResource);
    if (!root)
    {
        throw std::exception("Failed to create actor from resource");
        return nullptr;
    }

    // create the actor instance
    std::shared_ptr<Actor> actor{ new Actor(GetNextActorId()) };
    if (!actor->Init(root))
    {
        throw std::exception("Failed to initialize actor");
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
            // partially complete so it's not worth it.  Note that the pActor instance will be destroyed because it
            // will fall out of scope with nothing else pointing to it.
            throw std::exception("Failed to add component to actor");
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
            throw std::exception(("Component failed to initialize: " + std::string(name)).c_str());
            return nullptr;
        }
    }
    else
    {
        throw std::exception(("Couldn't find ActorComponent named " + std::string(name)).c_str());
        return nullptr;  // fail
    }

    // component will be NULL if the component wasn't found.  
    // This isn't necessarily an error since you might have a 
    // custom CreateComponent() function in a sub class.
    return component;
}