#include "RootSignature.h"

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return { pointWrap, pointClamp, linearWrap, linearClamp, anisotropicWrap, anisotropicClamp };
}


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

	static constexpr auto numTextureDesc = 1;
	auto numParams = numTextureDesc + numConstantBuffers;

	// Root parameter can be a table, root descriptor or root constants.
	std::vector<CD3DX12_ROOT_PARAMETER1> slotRootParameters(numParams);

	// textures (t0, ..., tn)
	CD3DX12_DESCRIPTOR_RANGE1 textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	slotRootParameters[0].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);

	// objects, pass, materials (b0, ..., bn)
	for (u32 i = 0; i < numConstantBuffers; i++)
		slotRootParameters[i + numTextureDesc].InitAsConstantBufferView(i); // specify cbuffer(b{i})

	// samplers (s0, ..., sn)
	auto samplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	auto rsDesc = CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(
		(u32) slotRootParameters.size(), slotRootParameters.data(), 
		(u32) samplers.size(), samplers.data(),
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