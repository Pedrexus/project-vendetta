#pragma once

#include <pch.h>
#include <types.h>

#include "../Resource.h"

class IResourceData
{
protected:
	Resource* m_resource = nullptr;
	
public:
	inline void SetResource(Resource r) { m_resource = &r; }
	std::string GetName() { return m_resource ? m_resource->GetName() : "Generic Resource Data"; };
	virtual void Parse(char* buffer, size size) {};
};

