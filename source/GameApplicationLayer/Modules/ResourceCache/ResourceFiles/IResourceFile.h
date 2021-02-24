#pragma once

#include <pch.h>
#include "../Resource.h"

// A resource file should be able to be 
//	opened and closed and provide the application
//	programmer access to resources
// The implementations can be .zip, .cab, ... (compression files)
class IResourceFile
{
public:
	virtual bool Open() = 0;
	virtual std::string GetResourceFilename(const int id) const = 0;
	virtual int GetRawResourceSize(const Resource& r) = 0;
	virtual bool GetRawResource(const Resource& r, char* buffer) = 0;
	virtual int GetNumResources() const = 0;
	virtual bool IsUsingDevelopmentDirectories(void) const = 0;
	virtual ~IResourceFile() { }
};


