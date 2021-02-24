#pragma once

#include <pch.h>

#include "../ResourceData/IResourceData.h"

// A resource loader specifies how a file inside the resource file should be read
//	The implementations can be for .ogg, .wav, .bitmap, ...  (data files)
class IResourceLoader
{
public:
	virtual std::string GetPattern() = 0;
	virtual bool UseRawFile() = 0;
	virtual bool DiscardRawBufferAfterLoad() = 0;
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) = 0;
	virtual IResourceData* LoadResource(char* rawBuffer, unsigned int rawSize) = 0;
	virtual unsigned int AddNullZero() { return false; }  // false => +0, true => +1
};