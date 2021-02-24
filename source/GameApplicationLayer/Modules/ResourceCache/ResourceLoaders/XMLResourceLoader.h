#pragma once

#include "IResourceLoader.h"
#include "../ResourceData/XMLResourceData.h"

class XMLResourceLoader : public IResourceLoader
{
public:
    // interface overrides
    bool UseRawFile() override { return false; }
    bool DiscardRawBufferAfterLoad() override { return true; }
    unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) override { return rawSize; }
    ResourceData::XML* LoadResource(char* rawBuffer, unsigned int rawSize) override;
    std::string GetPattern() override { return "*.xml"; }
};



