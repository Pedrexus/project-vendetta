#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "ResourceHandle.h"
#include "ResourceFiles/IResourceFile.h"
#include "ResourceLoaders/IResourceLoader.h"

typedef std::list<std::shared_ptr<ResourceHandle>> ResourceHandleList;
typedef std::map<std::string, std::shared_ptr<ResourceHandle>> ResourceHandleMap; // maps indentifiers to resource data
typedef std::list<std::shared_ptr<IResourceLoader>> ResourceLoaders;  // queue

class ResourceCache
{
	friend class ResourceHandle;

	ResourceHandleList m_lru;
	ResourceHandleMap m_resources;
	ResourceLoaders m_resourceLoaders;

	IResourceFile* m_file;

	unsigned int			m_cacheSize;			// total memory size
	unsigned int			m_allocated;			// total memory allocated

	unsigned int AvailableMemory() { return m_cacheSize - m_allocated; }

protected:
	bool MakeRoom(unsigned int size);
	char* Allocate(unsigned int size);
	void Free(std::shared_ptr<ResourceHandle> gonner);  // finds a resource and removes it from the cache.

	std::shared_ptr<ResourceHandle> Load(Resource* r);
	std::shared_ptr<ResourceHandle> Find(Resource* r);   // locates the right ResourceHandle given a Resource.
	void Update(std::shared_ptr<ResourceHandle> handle); // moves a ResourceHandle to the front

	void FreeOneResource();
	void MemoryHasBeenFreed(unsigned int size);

public:
	ResourceCache(const unsigned int size, IResourceFile* file);
	virtual ~ResourceCache();

	bool Init();
	void RegisterLoader(std::shared_ptr<IResourceLoader> loader);
	std::shared_ptr<ResourceHandle> GetHandle(Resource* r);
	int Preload(const std::string pattern, void (*progressCallback)(int, bool&));
	std::vector<std::string> Match(const std::string pattern);
	void Flush(void);
};