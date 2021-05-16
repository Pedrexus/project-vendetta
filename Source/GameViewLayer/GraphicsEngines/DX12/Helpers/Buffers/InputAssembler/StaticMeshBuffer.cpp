#include "StaticMeshBuffer.h"

void StaticMeshBuffer::Upload(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const Mesh* data)
{
	_VertexBufferGPU.Create(device, data->GetVerticesByteSize());
	_IndexBufferGPU.Create(device, data->GetIndicesByteSize<Index>());

	_VertexBufferGPU.Upload(cmdList, data->Vertices.data());

	auto indices = data->GetIndices<Index>();
	_IndexBufferGPU.Upload(cmdList, indices.data());

	_vbv.BufferLocation = _VertexBufferGPU->GetGPUVirtualAddress();
	_vbv.SizeInBytes = (u32) data->GetVerticesByteSize();
	_vbv.StrideInBytes = sizeof(Vertex);

	_ibv.BufferLocation = _IndexBufferGPU->GetGPUVirtualAddress();
	_ibv.SizeInBytes = (u32) data->GetIndicesByteSize<Index>();
	_ibv.Format = DXGI_FORMAT_R16_UINT;
}

// We can free this memory after we finish uploading to the GPU.
void StaticMeshBuffer::Dispose()
{
	_VertexBufferGPU.Dispose();
	_IndexBufferGPU.Dispose();
}