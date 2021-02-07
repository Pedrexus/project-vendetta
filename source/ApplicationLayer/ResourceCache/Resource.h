#pragma once

#include <utils.h>

class Resource
{
	std::string m_name;

public:
	Resource(const std::string& name);
	std::string GetName() { return m_name; }
};