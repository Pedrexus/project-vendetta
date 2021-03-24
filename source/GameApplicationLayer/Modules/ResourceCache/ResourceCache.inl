#pragma once

#include "../ResourceCache.h"
#include "../AudioSystem.h"

// explicit instantiations - avoid linker "unresolved external symbol"
template std::shared_ptr<ResourceData::XML> ResourceCache::GetData(const std::string filename);
template std::shared_ptr<ResourceData::Sound> ResourceCache::GetData(const std::string filename);