#include "ResourceHandle.h"

#include <macros.h>


ResourceHandle::ResourceHandle(Resource& resource, char* buffer, unsigned int size, ResourceCache* pResCache) :
	m_resource(resource),
	m_buffer(buffer),
	m_size(size),
	// m_extra(nullptr),
	m_pResCache(pResCache)
{};

ResourceHandle::~ResourceHandle()
{
	SAFE_DELETE_ARRAY(m_buffer);
	m_pResCache->MemoryHasBeenFreed(m_size);
}
