#pragma once

#include <pch.h>

class Resource
{
	std::string m_name;

public:
	Resource(const std::string& name);
	std::string GetName() const { return m_name; }
};