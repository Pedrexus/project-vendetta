#pragma once

#include "IMeshBuffer.h"
#include "../UploadBuffer.h"
#include "../../InputAssembler/Vertex.h"
#include "../../InputAssembler/Objects/Geometry.h"


class DynamicMeshBuffer : public IMeshBuffer
{
	using Index = u16;

	using VertexBufferGPU = UploadBuffer<Vertex>;
	using IndexBufferGPU = UploadBuffer<Index>;

	VertexBufferGPU _VB;
	IndexBufferGPU _IB;

public: 
	u64 VerticesCount;
	u64 IndicesCount;

	DynamicMeshBuffer() = default;
	DynamicMeshBuffer(const DynamicMeshBuffer & rhs) = delete;
	DynamicMeshBuffer& operator=(const DynamicMeshBuffer & rhs) = delete;

	void Upload(ID3D12Device* device, const Mesh* mesh)
	{
		VerticesCount = mesh->Vertices.size();
		IndicesCount = mesh->Indices.size();

		_VB.Create(device, VerticesCount);
		_IB.Create(device, IndicesCount);

		Update(mesh);
	}

	// TODO: create method to update specific vertices
	void Update(const Mesh* mesh)
	{
		for (auto i = 0; i < VerticesCount; i++)
			_VB.CopyToCPUBuffer(i, mesh->Vertices[i]);

		auto indices = mesh->GetIndices<Index>();
		for (auto i = 0; i < IndicesCount; i++)
			_IB.CopyToCPUBuffer(i, indices[i]);
	}

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const override { 
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = _VB.GetGPUVirtualAddress();
		vbv.SizeInBytes = (u32) VerticesCount * sizeof(Vertex);
		vbv.StrideInBytes = sizeof(Vertex);

		return vbv;
	};
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const override { 
		D3D12_INDEX_BUFFER_VIEW ibv = {};
		ibv.BufferLocation = _IB.GetGPUVirtualAddress();
		ibv.SizeInBytes = (u32) IndicesCount * sizeof(u16);
		ibv.Format = DXGI_FORMAT_R16_UINT;

		return ibv;
	};
};