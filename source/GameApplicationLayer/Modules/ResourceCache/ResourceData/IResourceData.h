#pragma once

#include <pch.h>
#include <types.h>

class IResourceData
{
public:
	virtual std::string GetName() = 0;
	virtual void Parse(char* buffer, size size) {};
};

