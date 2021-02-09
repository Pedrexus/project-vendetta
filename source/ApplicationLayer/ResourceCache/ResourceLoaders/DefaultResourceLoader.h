#pragma once

#include <ApplicationLayer/ResourceCache/ResourceLoaders/IResourceLoader.h>
#include <ApplicationLayer/ResourceCache/ResourceHandle.h>

class DefaultResourceLoader : public IResourceLoader
{
public:
	virtual bool UseRawFile() override { return true; }
	virtual std::string GetPattern() override { return "*"; }
	virtual bool DiscardRawBufferAfterLoad() override { return true; }
	virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) override { return rawSize; }
	virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, std::shared_ptr<ResourceHandle> handle) override { return true; }
};