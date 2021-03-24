#pragma once

#include <pch.h>

#include "Resource.h"
#include "ResourceCache.h"

#include "ResourceData/IResourceData.h"

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

	inline const std::string GetName() { return m_resource.GetName(); }
	inline unsigned int Size() const { return m_size; }
	inline char* Buffer() const { return m_buffer; }
	inline char* WritableBuffer() { return m_buffer; }

	inline void SetData(std::shared_ptr<IResourceData> data) { m_resourceData = data; data->SetResource(m_resource); }
	template<class T> 
	std::shared_ptr<T> GetData()
	{
		if (!m_resourceData)
			LOG_FATAL("Data was not loaded for handle of resource " + this->GetName());

		return std::dynamic_pointer_cast<T>(m_resourceData);
	}
};