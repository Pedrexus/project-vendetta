#pragma once

// avoiding circular dependence
class ResourceCache;
class ResourceHandle;
class IResourceLoader;

#include "ResourceCache/ResourceData/IResourceData.h"
#include "ResourceCache/ResourceData/XMLResourceData.h"

#include "ResourceCache/ResourceFiles/IResourceFile.h"
#include "ResourceCache/ResourceFiles/ResourceZipFile.h"

#include "ResourceCache/ResourceLoaders/IResourceLoader.h"
#include "ResourceCache/ResourceLoaders/DefaultResourceLoader.h"
#include "ResourceCache/ResourceLoaders/XMLResourceLoader.h"

#include "ResourceCache/Resource.h"
#include "ResourceCache/ResourceHandle.h"
#include "ResourceCache/ResourceCache.h"