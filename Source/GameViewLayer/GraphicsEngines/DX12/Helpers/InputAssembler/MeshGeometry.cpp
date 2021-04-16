#include "MeshGeometry.h"


MeshGeometry::MeshGeometry(std::vector<Vertex> vertices, std::vector<u16> indices, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	const u32 vbByteSize = vertices.size() * sizeof(Vertex);
	const u32 ibByteSize = indices.size() * sizeof(u16);

	m_VertexBufferGPU = std::make_unique<DefaultBuffer>(device, cmdList, vertices.data(), vbByteSize);
	m_IndexBufferGPU = std::make_unique<DefaultBuffer>(device, cmdList, indices.data(), ibByteSize);

	m_VertexByteStride = sizeof(Vertex);
	m_VertexBufferByteSize = vbByteSize;
	m_IndexFormat = DXGI_FORMAT_R16_UINT;
	m_IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	m_DrawArgs["box"] = submesh;
}

D3D12_VERTEX_BUFFER_VIEW MeshGeometry::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = m_VertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = m_VertexByteStride;
	vbv.SizeInBytes = m_VertexBufferByteSize;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW MeshGeometry::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = m_IndexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = m_IndexFormat;
	ibv.SizeInBytes = m_IndexBufferByteSize;

	return ibv;
}

D3D12_INPUT_LAYOUT_DESC MeshGeometry::SpecifyInputLayout()
{
	return { VertexInputLayout.data(), (u32) VertexInputLayout.size() };
}

// UNUSED - We can free this memory after we finish uploading to the GPU.
void MeshGeometry::DisposeUploaders()
{
	m_VertexBufferGPU->Dispose();
	m_IndexBufferGPU->Dispose();
}