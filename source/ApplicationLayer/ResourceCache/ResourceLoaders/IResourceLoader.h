#pragma once

#include <string>
#include <memory>

#include "../ResourceHandle.h"

// A resource loader specifies how a file inside the resource file should be read
//	The implementations can be for .ogg, .wav, .bitmap, ...  (data files)
class IResourceLoader
{
public:
	virtual std::string GetPattern() = 0;
	virtual bool UseRawFile() = 0;
	virtual bool DiscardRawBufferAfterLoad() = 0;
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) = 0;
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResourceHandle> handle) = 0;
	virtual unsigned int AddNullZero() { return false; }  // false => +0, true => +1
};