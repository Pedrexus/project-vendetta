#pragma once

#include "../../dx12pch.h"

#include "SubmeshGeometry.h"

/*
	Groups a vertex and index buffer together to
	define a group of geometry
*/
struct IMeshGeometry
{
	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	ComPtr<ID3DBlob> VertexBufferCPU;
	ComPtr<ID3DBlob> IndexBufferCPU;

	ComPtr<ID3D12Resource> VertexBufferGPU;
	ComPtr<ID3D12Resource> IndexBufferGPU;

	ComPtr<ID3D12Resource> VertexBufferUploader;
	ComPtr<ID3D12Resource> IndexBufferUploader;

	// Data about the buffers.
	u32 VertexByteStride = 0;
	u32 VertexBufferByteSize = 0;
	
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	u32 IndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	inline D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	inline D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv = {};
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish uploading to the GPU.
	void DisposeUploaders()
	{
		VertexBufferUploader.Reset();
		IndexBufferUploader.Reset();
	}
};