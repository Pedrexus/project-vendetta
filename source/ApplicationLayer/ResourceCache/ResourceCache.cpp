#include "ResourceCache.h"
#include "ResourceLoaders/DefaultResourceLoader.h"

#include <const.h>
#include <macros.h>


ResourceCache::ResourceCache(const unsigned int sizeInMb, IResourceFile* file) :
	m_cacheSize(sizeInMb* MEGABYTE),
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
		if (!handle) throw std::exception("Unable to load resource");
	}
	else
	{
		Update(handle);
	}
	return handle;
}

// TODO: understand this better, might be refactorable
std::shared_ptr<ResourceHandle> ResourceCache::Load(Resource* r)
{
	// Create a new resource and add it to the lru list and map

	std::shared_ptr<IResourceLoader> loader;
	std::shared_ptr<ResourceHandle> handle;

	// traverse m_resourceLoaders looking for the right loader
	// m_resourceLoaders goes from specific loaders to generic ones
	for (ResourceLoaders::iterator it = m_resourceLoaders.begin(); it != m_resourceLoaders.end(); ++it)
	{
		std::shared_ptr<IResourceLoader> testLoader = *it;

		if (WildcardMatch(testLoader->GetPattern().c_str(), r->GetName().c_str()))
		{
			loader = testLoader;
			break;
		}
	}

	if (!loader) throw std::exception("Default resource loader not found!");

	int rawSize = m_file->GetRawResourceSize(*r);

	if (rawSize < 0) throw std::exception("Resource size returned -1. Resource not found!");

	int allocSize = rawSize + loader->AddNullZero();
	char* rawBuffer = loader->UseRawFile() ? Allocate(allocSize) : new char[allocSize];
	memset(rawBuffer, 0, allocSize); // fills the first allocSize bytes in rawBuffer to 0

	if (rawBuffer == NULL || m_file->GetRawResource(*r, rawBuffer) == 0) throw std::exception("Resource cache out of memory");

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

		if (rawBuffer == NULL || buffer == NULL) throw std::exception("Resource cache out of memory");

		handle = std::shared_ptr<ResourceHandle>{ new ResourceHandle(*r, buffer, size, this) };
		bool success = loader->LoadResource(rawBuffer, rawSize, handle);

		// [mrmike] - This was added after the chapter went to copy edit. It is used for those
		//            resources that are converted to a useable format upon load, such as a compressed
		//            file. If the raw buffer from the resource file isn't needed, it shouldn't take up
		//            any additional memory, so we release it.
		//
		if (loader->DiscardRawBufferAfterLoad())
		{
			SAFE_DELETE_ARRAY(rawBuffer);
		}

		if (!success) throw std::exception("Resource cache out of memory");
	}

	if (handle)
	{
		m_lru.push_front(handle);
		m_resources[r->GetName()] = handle;
	}

	if (!loader) throw std::exception("Default resource loader not found!");
	return handle;		// ResCache is out of memory!
}

char* ResourceCache::Allocate(unsigned int size)
{
	if (!MakeRoom(size))
		return NULL;

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
			std::shared_ptr<ResourceHandle> handle = GetHandle(&r);
			++loaded;
		}

		if (progressCallback != NULL) progressCallback(i * 100 / numFiles, cancel);
	}
	return loaded;
}