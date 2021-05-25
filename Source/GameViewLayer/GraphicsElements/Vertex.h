#pragma once

#include <dx12pch.h>

struct Vertex
{
	XMFLOAT3 Position = {};
	XMFLOAT3 Normal = {};
	XMFLOAT3 Tangent = {};
	XMFLOAT2 Texture = {};
	XMFLOAT4 Color = {};

	static inline D3D12_INPUT_LAYOUT_DESC SpecifyInputLayout()
	{
		static std::vector<D3D12_INPUT_ELEMENT_DESC> VertexInputLayout =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		return { VertexInputLayout.data(), (u32) VertexInputLayout.size() };
	}

	Vertex() = default;

	Vertex(
		const aiVector3D& v
	) :
		Position(v.x, v.y, v.z)
	{}

	Vertex(
		const XMFLOAT3& p,
		const XMFLOAT4& c
	) :
		Position(p),
		Color(c)
	{};

	Vertex(
		const XMFLOAT3& p,
		const XMFLOAT3& n,
		const XMFLOAT3& t,
		const XMFLOAT2& uv) :
		Position(p),
		Normal(n),
		Tangent(t),
		Texture(uv)
	{}

	Vertex(
		f32 px, f32 py, f32 pz,
		f32 nx, f32 ny, f32 nz,
		f32 tx, f32 ty, f32 tz,
		f32 u, f32 v) :
		Position(px, py, pz),
		Normal(nx, ny, nz),
		Tangent(tx, ty, tz),
		Texture(u, v)
	{}
};