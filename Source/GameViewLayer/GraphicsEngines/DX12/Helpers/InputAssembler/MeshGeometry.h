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
	std::unique_ptr<DefaultBuffer> m_VertexBufferGPU;
	std::unique_ptr<DefaultBuffer> m_IndexBufferGPU;

	// Data about the buffers.
	u32 m_VertexByteStride = 0;
	u32 m_VertexBufferByteSize = 0;

	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R16_UINT;
	u32 m_IndexBufferByteSize = 0;

public:
	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, SubmeshGeometry> m_DrawArgs;

	MeshGeometry(std::vector<Vertex> vertices, std::vector<u16> indices, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	void DisposeUploaders();
};