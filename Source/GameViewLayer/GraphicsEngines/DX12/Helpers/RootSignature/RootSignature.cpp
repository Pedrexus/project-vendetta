#include "RootSignature.h"


// A root signature is an array of root parameters.

CD3DX12_DESCRIPTOR_RANGE1 RootSignature::SpecifyCBVTable(u32 baseShaderRegister)
{
	CD3DX12_DESCRIPTOR_RANGE1 cbvTable = {};
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, baseShaderRegister);
	return cbvTable;
}

ComPtr<ID3DBlob> RootSignature::SpecifyAndSerialize(u32 numConstantBuffers)
{
	if (numConstantBuffers > 5)
		LOG_WARNING("Setting more than 5 constant buffers might harm performance");

	auto numParams = numConstantBuffers;

	// Root parameter can be a table, root descriptor or root constants.
	std::vector<CD3DX12_ROOT_PARAMETER1> slotRootParameters(numParams);
	std::vector<CD3DX12_DESCRIPTOR_RANGE1> cbvTableArray(numParams);

	for (u32 i = 0; i < numConstantBuffers; i++)
	{
		cbvTableArray[i] = SpecifyCBVTable(i);
		slotRootParameters[i].InitAsDescriptorTable(1, &cbvTableArray[i]);
	}

	// A root signature is an array of root parameters.
	auto rsDesc = CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(
		numConstantBuffers, slotRootParameters.data(), 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	return Serialize(rsDesc);
}

RootSignature::RootSignature(ID3D12Device* device, u32 numConstantBuffers)
{
	auto serializedRootSig = SpecifyAndSerialize(numConstantBuffers);
	Create(device, serializedRootSig.Get());
}

ComPtr<ID3DBlob> RootSignature::Serialize(D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc)
{
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	auto hr = D3D12SerializeVersionedRootSignature(&rsDesc, &serializedRootSig, &errorBlob);

	if (errorBlob)
		LOG_ERROR((char*) errorBlob->GetBufferPointer());
	ThrowIfFailed(hr);

	return serializedRootSig;
}

void RootSignature::Create(ID3D12Device* device, ID3DBlob* serializedRootSig)
{
	ThrowIfFailed(device->CreateRootSignature(0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature))
	);
}