#pragma once

#include <memory>

#include <ApplicationLayer/ResourceCache/Resource.h>
#include <ApplicationLayer/ResourceCache/ResourceCache.h>

// TODO: add IResourceExtraData fields
class ResourceHandle
{
	friend class ResourceCache;

protected:
	Resource m_resource;
	char* m_buffer;
	unsigned int m_size;
	// std::shared_ptr<IResourceExtraData> m_extra;
	ResourceCache* m_pResCache;

public:
	ResourceHandle(Resource& resource, char* buffer, unsigned int size, ResourceCache* pResCache);
	virtual ~ResourceHandle();

	const std::string GetName() { return m_resource.GetName(); }
	unsigned int Size() const { return m_size; }
	char* Buffer() const { return m_buffer; }
	char* WritableBuffer() { return m_buffer; }

	// std::shared_ptr<IResourceExtraData> GetExtra() { return m_extra; }
	// void SetExtra(std::shared_ptr<IResourceExtraData> extra) { m_extra = extra; }
};