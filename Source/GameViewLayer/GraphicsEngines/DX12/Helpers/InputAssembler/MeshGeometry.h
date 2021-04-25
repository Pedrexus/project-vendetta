#pragma once

#include "../../dx12pch.h"

#include "../Buffers/DefaultBuffer.h"

#include "Vertex.h"
#include "SubmeshGeometry.h"

/*
	Groups a vertex and index buffer together to
	define a group of geometry
*/
class MeshGeometry
{
	std::unique_ptr<DefaultBuffer> _VertexBufferGPU;
	std::unique_ptr<DefaultBuffer> _IndexBufferGPU;

	// Data about the buffers.
	u32 _VertexByteStride = 0;
	u32 _VertexBufferByteSize = 0;

	DXGI_FORMAT _IndexFormat = DXGI_FORMAT_R16_UINT;
	u32 _IndexBufferByteSize = 0;

public:
	MeshGeometry(std::vector<Vertex>& vertices, std::vector<u16>& indices, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	MeshGeometry(MeshGeometry& rhs) = delete;
	MeshGeometry operator=(MeshGeometry& rhs) = delete;

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	void DisposeUploaders();
};