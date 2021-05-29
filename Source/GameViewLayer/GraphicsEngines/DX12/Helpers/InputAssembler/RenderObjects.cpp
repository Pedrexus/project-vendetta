#include "RenderObjects.h"

u32 GetTotalSize(RenderObjects::MeshMap& meshes, std::function<u32(RenderObjects::MeshMapItem)> lambda)
{
	return std::transform_reduce(LOOP_EXECUTION_POLICY, meshes.begin(), meshes.end(), 0u, std::plus<u32>{}, lambda);
}

u32 GetTotalVertexCount(RenderObjects::MeshMap& meshes)
{
	return GetTotalSize(meshes, [] (RenderObjects::MeshMapItem i) { return (u32) i.second->Vertices.size(); });
};

u32 GetTotalIndexCount(RenderObjects::MeshMap& meshes)
{
	return GetTotalSize(meshes, [] (RenderObjects::MeshMapItem i) { return (u32) i.second->Indices.size(); });
};

std::vector<Mesh::Vertex> BuildPackedVertexVector(RenderObjects::MeshMap& meshes)
{
	std::vector<Mesh::Vertex> vertices;
	vertices.reserve(GetTotalVertexCount(meshes));

	for (auto& [_, mesh] : meshes)
	{
		auto begin = std::make_move_iterator(mesh->Vertices.begin());
		auto end = std::make_move_iterator(mesh->Vertices.end());
		vertices.insert(vertices.end(), begin, end);
	}

	return vertices;
}

std::vector<Mesh::Index> BuildPackedIndexVector(RenderObjects::MeshMap& meshes)
{
	std::vector<Mesh::Index> indices;
	indices.reserve(GetTotalIndexCount(meshes));

	for (auto& [_, mesh] : meshes)
	{
		auto begin = std::make_move_iterator(mesh->Indices.begin());
		auto end = std::make_move_iterator(mesh->Indices.end());
		indices.insert(indices.end(), begin, end);
	}

	return indices;
}

RenderObjects::RenderObjects(MeshMap& staticMeshes, MeshMap& dynamicMeshes, Objects& table, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	CreateSubmeshes(staticMeshes);
	auto vertices = BuildPackedVertexVector(staticMeshes);
	auto indices = BuildPackedIndexVector(staticMeshes);

	Mesh mesh = { vertices, indices };
	_StaticMeshBuffer.Upload(device, cmdList, &mesh);

	for (auto& [name, mesh] : dynamicMeshes)
	{
		auto buffers = std::vector<DynamicMeshBuffer>(Settings::GetInt("graphics-frame-resources"));
		for (auto& buf : buffers)
			buf.Upload(device, mesh);
		_DynamicMeshBuffers[name] = std::move(buffers);
	}

	CreateObjects(table);
}

void RenderObjects::CreateObjects(Objects& objects)
{
	_RenderItems.reserve(objects.size());

	for (auto& obj : objects)
	{
		auto& name = obj.Name;
		if (_Submeshes.contains(name))
			_RenderItems.push_back({ obj, &_Submeshes[name], &_StaticMeshBuffer });
		else
		{
			auto& buffer = _DynamicMeshBuffers[name][0];
			_RenderItems.push_back({ obj, &buffer, buffer.IndicesCount });
		}
	}
}

std::vector<RenderItem>& RenderObjects::items()
{
	return _RenderItems;
}

void RenderObjects::CreateSubmeshes(MeshMap& meshes)
{
	_Submeshes.reserve(meshes.size());

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

void RenderObjects::UpdateMesh(MeshName name, u8 frameIndex, Mesh* newMesh)
{
	auto& buffer = _DynamicMeshBuffers[name][frameIndex];

	if (newMesh)
		buffer.Update(newMesh);

	for (auto& obj : _RenderItems)
		if (obj.Name == name)
			obj.Buffer = &buffer;
}