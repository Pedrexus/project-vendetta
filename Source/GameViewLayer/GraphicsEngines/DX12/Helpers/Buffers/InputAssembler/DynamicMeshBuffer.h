#pragma once

#include "IMeshBuffer.h"
#include "../DynamicBuffer.h"
#include "../../InputAssembler/Vertex.h"
#include "../../InputAssembler/Objects/Geometry.h"


class DynamicMeshBuffer : public IMeshBuffer
{
	using Index = u16;

	using VertexBufferGPU = DynamicBuffer<Vertex>;
	using IndexBufferGPU = DynamicBuffer<Index>;

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
		_VB.Upload(mesh->Vertices.data());
		_IB.Upload(mesh->GetIndices<Index>().data());
	}

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const override { 
		auto view = _VB.GetBufferView();

		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = view.BufferLocation;
		vbv.SizeInBytes = view.TotalByteSize;
		vbv.StrideInBytes = view.ElementByteSize;

		return vbv;
	};
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const override { 
		auto view = _IB.GetBufferView();

		D3D12_INDEX_BUFFER_VIEW ibv = {};
		ibv.BufferLocation = view.BufferLocation;
		ibv.SizeInBytes = view.TotalByteSize;
		ibv.Format = DXGI_FORMAT_R16_UINT;

		return ibv;
	};
};