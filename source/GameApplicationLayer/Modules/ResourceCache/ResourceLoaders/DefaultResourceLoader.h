#pragma once


class DefaultResourceLoader : public IResourceLoader
{
public:
	bool UseRawFile() override { return true; }
	std::string GetPattern() override { return "*"; }
	bool DiscardRawBufferAfterLoad() override { return true; }
	unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) override { return rawSize; }
	IResourceData* LoadResource(char* rawBuffer, unsigned int rawSize) override { return nullptr; }
};