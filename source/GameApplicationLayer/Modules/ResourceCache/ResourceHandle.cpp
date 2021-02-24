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

std::shared_ptr<IResourceData> ResourceHandle::GetData()
{
	if (!m_resourceData)
		LOG_FATAL("Data was not loaded for handle of resource " + this->GetName());

	return m_resourceData;
}
