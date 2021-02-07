#pragma once

#include <ApplicationLayer/Actors.h>

typedef UINT ActorId;

// Its entire purpose is to manage and maintain components
class Actor
{
	friend class ActorFactory;

	// COMPOSITION: each actor holds a few components
	typedef std::map<ActorComponentId, std::shared_ptr<ActorComponent>> ActorComponentMap;

	ActorId m_id;	// unique id for the actor
	ActorComponentMap m_componentMap; // all components this actor has

	// called exclusively by ActorFactory
	void AddComponent(std::shared_ptr<ActorComponent> component);

public:
	explicit Actor(ActorId id);
	~Actor(void);

	bool Init(tinyxml2::XMLElement* node);
	void PostInit(void);
	void Destroy(void); // explicitly clears out the component map
	void Update(int deltaMs);

	// accessors
	inline ActorId GetId(void) const { return m_id; }

	// this function has the possibility of being called hundreds
	// or even thousands of times each frame.
	// It can be optimized for faster performance.
	template <class ComponentType> 
	std::weak_ptr<ComponentType> GetComponent(ActorComponentId name)
	{
		ActorComponentMap::iterator findIterator = m_componentMap.find(name);
		if (findIterator != m_componentMap.end())
		{
			auto &baseComponent = findIterator->second;
			std::shared_ptr<ComponentType> subComponent = std::static_pointer_cast<ComponentType>(baseComponent);  // cast to subclass version of the pointer
			std::weak_ptr<ComponentType> weakSubComponent = subComponent;  // convert strong pointer to weak pointer
			return weakSubComponent;  // return the weak pointer
		}
		else
		{
			return nullptr;
		}
	}

};

