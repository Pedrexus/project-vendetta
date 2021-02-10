#include "XMLResourceLoader.h"

#include <macros.h>

ResourceData::XML* XMLResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize)
{
	if (rawSize <= 0)
		return nullptr;

	auto resourceData = NEW ResourceData::XML();
	resourceData->Parse(rawBuffer, rawSize);

	return resourceData;
}


