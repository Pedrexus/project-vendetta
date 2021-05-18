#pragma once



#include "../Buffers/InputAssembler/StaticMeshBuffer.h"
#include "../Buffers/InputAssembler/DynamicMeshBuffer.h"

#include "Objects/Geometry.h"
#include "SubmeshGeometry.h"
#include "RenderItem.h"

class RenderObjects
{
public:
	using MeshName = const std::string;
	using MeshData = std::pair<Mesh*, Mesh::BufferType>;

	using WorldMap = std::vector<std::pair<MeshName, XMMATRIX>>;

	using MeshMap = std::map<MeshName, Mesh*>;
	using MeshMapItem = MeshMap::iterator::value_type;

	using FrameDynamicBuffer = std::vector<DynamicMeshBuffer>;

private:
	void CreateObjects(WorldMap& table);
	void CreateSubmeshes(MeshMap& meshes);

protected:
	std::vector<RenderItem> _RenderItems;

	// static meshes
	std::unordered_map<std::string, SubmeshGeometry> _Submeshes;
	StaticMeshBuffer _StaticMeshBuffer;

	// dynamic meshes
	std::unordered_map<std::string, FrameDynamicBuffer> _DynamicMeshBuffers;


public:
	RenderObjects(MeshMap& staticMeshes, MeshMap& dynamicMeshes, WorldMap& table, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	RenderObjects(RenderObjects& rhs) = delete;
	RenderObjects operator=(RenderObjects& rhs) = delete;

	void UpdateMesh(MeshName name, u8 frameIndex, Mesh* mesh);

	std::vector<RenderItem>& items();
};