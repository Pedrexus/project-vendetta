#include "RootSignature.h"


ComPtr<ID3DBlob> Serialize(D3D12_VERSIONED_ROOT_SIGNATURE_DESC& rsDesc)
{
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	auto hr = D3D12SerializeVersionedRootSignature(&rsDesc, &serializedRootSig, &errorBlob);

	if (errorBlob)
		LOG_ERROR((char*) errorBlob->GetBufferPointer());
	ThrowIfFailed(hr);

	return serializedRootSig;
}

CD3DX12_DESCRIPTOR_RANGE1 SpecifyCBVRange(u32 baseShaderRegister)
{
	// TODO: I want to understand how to use a single range
	//  technically, I could use numDescriptors = numConstantBuffers
	return CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, baseShaderRegister);
}

ComPtr<ID3DBlob> RootSignature::SpecifyAndSerialize(u32 numConstantBuffers)
{
	if (numConstantBuffers > 5)
		LOG_WARNING("Setting more than 5 constant buffers might harm performance");

	auto numParams = numConstantBuffers;

	// Root parameter can be a table, root descriptor or root constants.
	std::vector<CD3DX12_ROOT_PARAMETER1> slotRootParameters(numParams);

	for (u32 i = 0; i < numConstantBuffers; i++)
		slotRootParameters[i].InitAsConstantBufferView(i); // specify cbuffer(b{i})

	// A root signature is an array of root parameters.
	auto rsDesc = CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(
		(u32) slotRootParameters.size(), slotRootParameters.data(), 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	return Serialize(rsDesc);
}

RootSignature::RootSignature(ID3D12Device* device, u32 numConstantBuffers)
{
	auto serializedRootSig = SpecifyAndSerialize(numConstantBuffers);
	Create(device, serializedRootSig.Get());
}

void RootSignature::Create(ID3D12Device* device, ID3DBlob* serializedRootSig)
{
	ThrowIfFailed(device->CreateRootSignature(0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature))
	);
}