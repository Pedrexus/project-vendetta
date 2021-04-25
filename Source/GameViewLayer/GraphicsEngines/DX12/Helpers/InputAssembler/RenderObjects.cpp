#include "RenderObjects.h"

RenderObjects::RenderObjects(MeshMap& meshes, WorldMap& table, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	_Vertices.reserve(GetTotalVertexCount(meshes));
	BuildPackedVertexVector(meshes);

	_Indices.reserve(GetTotalIndexCount(meshes));
	BuildPackedIndexVector(meshes);

	_Submeshes.reserve(meshes.size());
	CreateSubmeshes(meshes);

	_Objects.reserve(table.size());
	CreateObjects(table);

	_MeshGeometryGroup = std::make_unique<MeshGeometry>(_Vertices, _Indices, device, cmdList);

	VertexBufferView = _MeshGeometryGroup->GetVertexBufferView();
	IndexBufferView = _MeshGeometryGroup->GetIndexBufferView();
}

void RenderObjects::CreateObjects(RenderObjects::WorldMap& table)
{
	u32 index = 0;
	for (auto& [name, world] : table)
		_Objects.push_back({ index++, world, _Submeshes[name] });
}

std::vector<RenderItem>& RenderObjects::items()
{
	return _Objects;
}

RenderObjects::~RenderObjects()
{
	_MeshGeometryGroup->DisposeUploaders();
}

void RenderObjects::CreateSubmeshes(MeshMap& meshes)
{
	u32 vertexOffset = 0;
	u32 indexOffset = 0;
	for (auto& [name, mesh] : meshes)
	{
		SubmeshGeometry submesh;
		submesh.IndexCount = (u32) mesh->Indices.size();
		submesh.BaseVertexLocation = vertexOffset;
		submesh.StartIndexLocation = indexOffset;
		_Submeshes[name] = submesh;

		vertexOffset += (u32) mesh->Vertices.size();
		indexOffset += (u32) mesh->Indices.size();
	}
}

u32 GetTotalSize(RenderObjects::MeshMap& meshes, std::function<u32(RenderObjects::MeshMapItem)> lambda)
{
	return std::transform_reduce(LOOP_EXECUTION_POLICY, meshes.begin(), meshes.end(), 0u, std::plus<u32>{}, lambda);
}

u32 RenderObjects::GetTotalVertexCount(MeshMap& meshes)
{
	return GetTotalSize(meshes, [] (MeshMapItem i) { return (u32) i.second->Vertices.size(); });
};

u32 RenderObjects::GetTotalIndexCount(MeshMap& meshes)
{
	return GetTotalSize(meshes, [] (MeshMapItem i) { return (u32) i.second->Indices.size(); });
}

void RenderObjects::BuildPackedVertexVector(MeshMap& meshes)
{
	for (auto& [_, mesh] : meshes)
	{
		auto begin = std::make_move_iterator(mesh->Vertices.begin());
		auto end = std::make_move_iterator(mesh->Vertices.end());
		_Vertices.insert(_Vertices.end(), begin, end);
	}
}

void RenderObjects::BuildPackedIndexVector(MeshMap& meshes)
{
	for (auto& [_, mesh] : meshes)
	{
		auto meshIndices = mesh->GetIndices<u16>();
		auto begin = std::make_move_iterator(meshIndices.begin());
		auto end = std::make_move_iterator(meshIndices.end());
		_Indices.insert(_Indices.end(), begin, end);
	}
}
