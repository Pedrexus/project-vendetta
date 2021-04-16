#pragma once

#include "../../dx12pch.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
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
