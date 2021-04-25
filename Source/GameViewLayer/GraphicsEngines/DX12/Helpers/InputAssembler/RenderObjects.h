#pragma once

#include <numeric>

#include "Objects/Geometry.h"
#include "MeshGeometry.h"
#include "RenderItem.h"

class RenderObjects
{
public:
	using WorldMap = std::vector<std::pair<std::string, XMMATRIX>>;
	using MeshMap = std::map<std::string, Mesh*>;
	using MeshMapItem = std::pair<const std::string, Mesh*>;

private:
	void CreateObjects(WorldMap& table);
	void CreateSubmeshes(MeshMap& meshes);

	u32 GetTotalVertexCount(MeshMap& meshes);
	u32 GetTotalIndexCount(MeshMap& meshes);

	void BuildPackedVertexVector(MeshMap& meshes);
	void BuildPackedIndexVector(MeshMap& meshes);

protected:
	std::unordered_map<std::string, SubmeshGeometry> _Submeshes;
	std::vector<Vertex> _Vertices;
	std::vector<u16> _Indices;
	std::vector<RenderItem> _Objects;
	std::unique_ptr<MeshGeometry> _MeshGeometryGroup;

public:
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

public:
	RenderObjects(MeshMap& meshes, WorldMap& table, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	RenderObjects(RenderObjects& rhs) = delete;
	RenderObjects operator=(RenderObjects& rhs) = delete;
	~RenderObjects();

	std::vector<RenderItem>& items();
};