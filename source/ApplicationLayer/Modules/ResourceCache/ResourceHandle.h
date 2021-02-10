#pragma once

#include <pch.h>

#include "../ResourceCache.h"

// TODO: add IResourceExtraData fields
class ResourceHandle
{
	friend class ResourceCache;

protected:
	Resource m_resource;
	char* m_buffer;
	unsigned int m_size;

	ResourceCache* m_pResCache;

	std::shared_ptr<IResourceData> m_resourceData;

public:
	ResourceHandle(Resource& resource, char* buffer, unsigned int size, ResourceCache* pResCache);
	virtual ~ResourceHandle();

	const std::string GetName() { return m_resource.GetName(); }
	unsigned int Size() const { return m_size; }
	char* Buffer() const { return m_buffer; }
	char* WritableBuffer() { return m_buffer; }

	std::shared_ptr<IResourceData> GetData() { return m_resourceData; }
	void SetData(std::shared_ptr<IResourceData> data) { m_resourceData = data; }
};