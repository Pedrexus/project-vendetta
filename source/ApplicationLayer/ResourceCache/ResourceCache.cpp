#include "ResourceCache.h"
#include "ResourceLoaders/DefaultResourceLoader.h"

#include <const.h>
#include <macros.h>
#include <helpers.h>

ResourceCache::ResourceCache(const unsigned int size, IResourceFile* file) :
	m_cacheSize(size),
	m_allocated(0),
	m_file(file)
{}

ResourceCache::~ResourceCache()
{
	while (!m_lru.empty())
	{
		FreeOneResource();
	}
	SAFE_DELETE(m_file);
};

bool ResourceCache::Init()
{
	if (m_file->Open())
	{
		// the most generic loaders come last in the list and the most specific loaders come first
		RegisterLoader(std::shared_ptr<IResourceLoader>{ new DefaultResourceLoader() });

		return true;
	}
	return false;
}

void ResourceCache::RegisterLoader(std::shared_ptr<IResourceLoader> loader)
{
	m_resourceLoaders.push_front(loader);
}

std::shared_ptr<ResourceHandle> ResourceCache::GetHandle(Resource* r)
{
	auto handle = Find(r);
	if (handle == NULL)
	{
		handle = Load(r);
		if (!handle)
		{
			LOG_ERROR("Unable to load resource");
			return nullptr;
		}
	}
	else
	{
		Update(handle);
	}
	return handle;
}

std::shared_ptr<ResourceHandle> ResourceCache::Find(Resource* r)
{
	auto i = m_resources.find(r->GetName());
	if (i == m_resources.end())
		return nullptr;

	return i->second;
}

// Creates a new resource and add it to the lru list and map
std::shared_ptr<ResourceHandle> ResourceCache::Load(Resource* r)
{
	std::shared_ptr<IResourceLoader> loader;
	std::shared_ptr<ResourceHandle> handle;

	// traverse m_resourceLoaders looking for the right loader
	// m_resourceLoaders goes from specific loaders to generic ones
	auto& loader = *std::find_if(
		m_resourceLoaders.begin(), 
		m_resourceLoaders.end(), 
		[r] (std::shared_ptr<IResourceLoader> testLoader) { 
			WildcardMatch(testLoader->GetPattern().c_str(), r->GetName().c_str());
		}
	);

	if (!loader)
	{
		LOG_ERROR("Default resource loader not found!");
		return nullptr;
	}

	int rawSize = m_file->GetRawResourceSize(*r);
	if (rawSize < 0)
	{
		LOG_ERROR("Resource size returned -1. Resource not found!");
		return nullptr;
	}

	// allocates an empty string
	int allocSize = rawSize + loader->AddNullZero();
	char* rawBuffer = loader->UseRawFile() ? Allocate(allocSize) : new char[allocSize];
	memset(rawBuffer, 0, allocSize); // fills the first allocSize bytes in rawBuffer to 0

	if (rawBuffer == NULL || m_file->GetRawResource(*r, rawBuffer) == 0)
	{
		LOG_ERROR("Resource cache out of memory");
		return nullptr;
	}

	char* buffer = NULL;
	unsigned int size = 0;

	if (loader->UseRawFile())
	{
		buffer = rawBuffer;
		handle = std::shared_ptr<ResourceHandle>{ new ResourceHandle(*r, buffer, rawSize, this) };
	}
	else
	{
		size = loader->GetLoadedResourceSize(rawBuffer, rawSize);
		buffer = Allocate(size);

		if (rawBuffer == NULL || buffer == NULL)
		{
			LOG_ERROR("Resource cache out of memory");
			return nullptr;
		}

		handle = std::shared_ptr<ResourceHandle>{ new ResourceHandle(*r, buffer, size, this) };
		bool success = loader->LoadResource(rawBuffer, rawSize, handle);

		// If the raw buffer from the resource file isn't needed, it shouldn't take up
		// any additional memory, so we release it.
		if (loader->DiscardRawBufferAfterLoad())
			SAFE_DELETE_ARRAY(rawBuffer);

		if (!success)
		{
			LOG_ERROR("Resource cache out of memory");
			return nullptr;
		}
	}

	if (handle)
	{
		m_lru.push_front(handle);
		m_resources[r->GetName()] = handle;
	}

	return handle;
}

// moves handle to front or lru
void ResourceCache::Update(std::shared_ptr<ResourceHandle> handle)
{
	m_lru.remove(handle);
	m_lru.push_front(handle);
}

char* ResourceCache::Allocate(unsigned int size)
{
	if (!MakeRoom(size))
		return nullptr;

	char* mem = new char[size];
	if (mem)
		m_allocated += size;

	return mem;
}

bool ResourceCache::MakeRoom(unsigned int size)
{
	if (size > m_cacheSize)
		return false;

	// return null if there's no possible way to allocate the memory
	while (size > AvailableMemory())
	{
		// The cache is empty, and there's still not enough room.
		if (m_lru.empty())
			return false;

		FreeOneResource();
	}

	return true;
}

void ResourceCache::FreeOneResource()
{
	auto gonner = m_lru.end(); // last item is NULL
	gonner--; // true last item

	std::shared_ptr<ResourceHandle> handle = *gonner;

	m_lru.pop_back();
	m_resources.erase(handle->m_resource.GetName());

	// Note - you can't change the resource cache size yet - the resource bits could still actually be
	// used by some subsystem holding onto the ResourceHandle. Only when it goes out of scope can the memory
	// be actually free again.
}

int ResourceCache::Preload(const std::string pattern, void (*progressCallback)(int, bool&))
{
	if (m_file == NULL)
		return 0;

	int numFiles = m_file->GetNumResources();
	int loaded = 0;
	bool cancel = false;
	for (int i = 0; i < numFiles; ++i)
	{
		Resource r{ m_file->GetResourceName(i) };

		if (WildcardMatch(pattern.c_str(), r.GetName().c_str()))
		{
			auto handle = GetHandle(&r);
			++loaded;
		}

		if (progressCallback != NULL) 
			progressCallback(i * 100 / numFiles, cancel);
	}
	return loaded;
}

void ResourceCache::Free(std::shared_ptr<ResourceHandle> gonner)
{
	m_lru.remove(gonner);
	m_resources.erase(gonner->m_resource.GetName());

	// Note - the resource might still be in use by something,
	// so the cache can't actually count the memory freed until the
	// ResHandle pointing to it is destroyed.
}

void ResourceCache::MemoryHasBeenFreed(unsigned int size)
{
	m_allocated -= size;
}

//    Frees every handle in the cache - this would be good to call if you are loading a new
//    level, or if you wanted to force a refresh of all the data in the cache - which might be 
//    good in a development environment.
void ResourceCache::Flush()
{
	while (!m_lru.empty())
	{
		Free(m_lru.front());
		m_lru.pop_front();
	}
}

//   Searches the resource cache assets for files matching the pattern. Useful for providing a 
//   a list of levels for a main menu screen, for example.
std::vector<std::string> ResourceCache::Match(const std::string pattern)
{
	std::vector<std::string> matchingNames;
	if (m_file == NULL)
		return matchingNames;

	int numFiles = m_file->GetNumResources();
	for (int i = 0; i < numFiles; ++i)
	{
		std::string name = m_file->GetResourceName(i);
		std::transform(name.begin(), name.end(), name.begin(), (int(*)(int)) std::tolower);
		if (WildcardMatch(pattern.c_str(), name.c_str()))
		{
			matchingNames.push_back(name);
		}
	}
	return matchingNames;
}