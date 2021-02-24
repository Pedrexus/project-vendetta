#include "Actor.h"

#include <macros.h>

Actor::Actor(ActorId id) :
	m_id(id),
	m_type("?"),
	m_resourceName("?")
{}

Actor::~Actor(void)
{
	LOG("Actor", "Destroying Actor " + std::to_string(m_id));

	if (!m_componentMap.empty())
		LOG_ERROR("Actor destroyed without calling Actor::Destroy()");
}

bool Actor::Init(tinyxml2::XMLElement* node)
{
	LOG("Actor", "Initializing Actor " + std::to_string(m_id));

	m_type = node->Attribute("type");
	m_resourceName = node->Attribute("resource");

	return true;
}

void Actor::PostInit(void)
{
	for (auto& [id, component] : m_componentMap)
		component->PostInit();
}

void Actor::Update(milliseconds dt)
{
	for (auto& [id, component] : m_componentMap)
		component->Update(dt);
}

void Actor::AddComponent(std::shared_ptr<ActorComponent> component)
{
	const auto& [it, success] = m_componentMap.insert({ component->GetName(), component });

	if (success)
		LOG("Actor", "Added Component " + component->GetName() + " to Actor " + std::to_string(m_id));
	else
		LOG_ERROR("Unable to add Component " + component->GetName() + " to Actor " + std::to_string(m_id));
}