#pragma once

#include "IMeshBuffer.h"
#include "../StaticBuffer.h"

/*
	Groups a vertex and index buffer together to
	define a group of geometry
*/
class StaticMeshBuffer : public IMeshBuffer
{
	StaticBuffer _VertexBufferGPU;
	StaticBuffer _IndexBufferGPU;

	// Data about the buffers.
	D3D12_VERTEX_BUFFER_VIEW _vbv;
	D3D12_INDEX_BUFFER_VIEW _ibv;

public:
	StaticMeshBuffer() = default;
	StaticMeshBuffer(StaticMeshBuffer& rhs) = delete;
	StaticMeshBuffer operator=(StaticMeshBuffer& rhs) = delete;

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const override { return _vbv; };
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const override { return _ibv; };

	void Upload(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const Mesh* data);
	void Dispose();
};