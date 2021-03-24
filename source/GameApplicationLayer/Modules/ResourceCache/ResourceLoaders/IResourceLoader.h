#pragma once

#include <pch.h>
#include <types.h>

#include "../ResourceData/IResourceData.h"

class ResourceHandle;

// A resource loader specifies how a file inside the resource file should be read
//	The implementations can be for .ogg, .wav, .bitmap, ...  (data files)
class IResourceLoader
{
public:
	virtual std::string GetPattern() = 0;
	virtual bool UseRawFile() = 0;
	virtual bool DiscardRawBufferAfterLoad() = 0;
	virtual size GetLoadedResourceSize(char* rawBuffer, size rawSize) = 0;
	virtual bool AddNullZero() { return false; }  // false => +0, true => +1

	// The loader has to implement at least one
	virtual IResourceData* LoadResource(char* rawBuffer, size rawSize, std::shared_ptr<ResourceHandle> handle) = 0;
};