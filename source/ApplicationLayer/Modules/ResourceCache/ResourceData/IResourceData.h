#pragma once

#include <pch.h>

class IResourceData
{
public:
	virtual std::string GetName() = 0;
	virtual void Parse(char* buffer, unsigned int size) = 0;
};

