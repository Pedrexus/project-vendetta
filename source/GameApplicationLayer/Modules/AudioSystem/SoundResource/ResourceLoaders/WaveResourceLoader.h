#pragma once

#include "../ResourceData/SoundResourceData.h"
#include "../../../ResourceCache.h"

class WaveResourceLoader : public IResourceLoader
{
protected:
	ResourceData::Sound* ParseWave(char* wavStream, size bufferSize, std::shared_ptr<ResourceHandle> handle);

public:
	std::string GetPattern() override { return "*.wav"; }
	bool UseRawFile() override { return false; }
	bool DiscardRawBufferAfterLoad() override { return true; }
	size GetLoadedResourceSize(char* rawBuffer, size rawSize) override;

	ResourceData::Sound* LoadResource(char* rawBuffer, size rawSize, std::shared_ptr<ResourceHandle> handle) override;
};