#include "MeshGeometry.h"


MeshGeometry::MeshGeometry(std::vector<Vertex>& vertices, std::vector<u16>& indices, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	const auto vbByteSize = (u32) vertices.size() * sizeof(Vertex);
	const auto ibByteSize = (u32) indices.size() * sizeof(u16);

	_VertexBufferGPU = std::make_unique<DefaultBuffer>(device, cmdList, vertices.data(), vbByteSize);
	_IndexBufferGPU = std::make_unique<DefaultBuffer>(device, cmdList, indices.data(), ibByteSize);

	_VertexByteStride = sizeof(Vertex);
	_VertexBufferByteSize = vbByteSize;
	_IndexFormat = DXGI_FORMAT_R16_UINT;
	_IndexBufferByteSize = ibByteSize;
}

D3D12_VERTEX_BUFFER_VIEW MeshGeometry::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = _VertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = _VertexByteStride;
	vbv.SizeInBytes = _VertexBufferByteSize;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW MeshGeometry::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = _IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = _IndexFormat;
	ibv.SizeInBytes = _IndexBufferByteSize;

	return ibv;
}

// UNUSED - We can free this memory after we finish uploading to the GPU.
void MeshGeometry::DisposeUploaders()
{
	_VertexBufferGPU->Dispose();
	_IndexBufferGPU->Dispose();
}