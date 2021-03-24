#pragma once

class IResourceLoader;

#include "IResourceLoader.h"

class DefaultResourceLoader : public IResourceLoader
{
public:
	bool UseRawFile() override { return true; }
	std::string GetPattern() override { return "*"; }
	bool DiscardRawBufferAfterLoad() override { return true; }
	size GetLoadedResourceSize(char* rawBuffer, size rawSize) override { return rawSize; }
	IResourceData* LoadResource(char* rawBuffer, size rawSize, std::shared_ptr<ResourceHandle> handle) override { return nullptr; }
};