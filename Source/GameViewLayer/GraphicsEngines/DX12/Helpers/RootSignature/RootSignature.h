#pragma once

#include "../../dx12pch.h"

/*
	Shader programs typically require resources as input (constant buffers,
	textures, samplers).  The root signature defines the resources the shader
	programs expect.  If we think of the shader programs as a function, and
	the input resources as function parameters, then the root signature can be
	thought of as defining the function signature.
*/
class RootSignature
{
	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

protected:
	ComPtr<ID3DBlob> SpecifyAndSerialize(u32 numConstantBuffers);
	ComPtr<ID3DBlob> Serialize(D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc);
	void Create(ID3D12Device* device, ID3DBlob* serializedRootSig);

	CD3DX12_DESCRIPTOR_RANGE1 SpecifyCBVTable(u32 baseShaderRegister);

public:
	RootSignature(ID3D12Device* device, u32 numConstantBuffers);
	RootSignature(RootSignature& rhs) = delete;
	RootSignature operator=(RootSignature& rhs) = delete;

	inline ID3D12RootSignature* Get() { return m_RootSignature.Get(); }
};