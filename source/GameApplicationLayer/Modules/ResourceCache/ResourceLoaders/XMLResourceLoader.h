#pragma once

#include <types.h>

#include "IResourceLoader.h"
#include "../ResourceData/XMLResourceData.h"

class XMLResourceLoader : public IResourceLoader
{
public:
    // interface overrides
    bool UseRawFile() override { return false; }
    bool DiscardRawBufferAfterLoad() override { return true; }
    size GetLoadedResourceSize(char* rawBuffer, size rawSize) override { return rawSize; }
    ResourceData::XML* LoadResource(char* rawBuffer, size rawSize, std::shared_ptr<ResourceHandle> handle) override;
    std::string GetPattern() override { return "*.xml"; }
};



