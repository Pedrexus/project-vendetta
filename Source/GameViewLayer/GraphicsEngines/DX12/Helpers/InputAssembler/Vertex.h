#pragma once

#include "../../dx12pch.h"


struct Vertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT4 Color = XMFLOAT4(DirectX::Colors::DarkGreen);
    // DirectX::XMFLOAT3 Normal;
    // DirectX::XMFLOAT3 Tangent;
    // DirectX::XMFLOAT2 Texture;

    Vertex() :
        Position{}
        // Normal{},
        // Tangent{},
        // Texture{}
    {}

    Vertex(
        const DirectX::XMFLOAT3& p,
        const DirectX::XMFLOAT3& n,
        const DirectX::XMFLOAT3& t,
        const DirectX::XMFLOAT2& uv) :
        Position(p)
        // Normal(n),
        // Tangent(t),
        // Texture(uv)
    {}
    Vertex(
        f32 px, f32 py, f32 pz,
        f32 nx, f32 ny, f32 nz,
        f32 tx, f32 ty, f32 tz,
        f32 u, f32 v) :
        Position(px, py, pz)
        // Normal(nx, ny, nz),
        // Tangent(tx, ty, tz),
        // Texture(u, v)
    {}
};

inline D3D12_INPUT_LAYOUT_DESC SpecifyInputLayout()
{
	static std::vector<D3D12_INPUT_ELEMENT_DESC> VertexInputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	return { VertexInputLayout.data(), (u32) VertexInputLayout.size() };
}
