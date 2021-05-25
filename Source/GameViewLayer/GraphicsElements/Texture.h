#pragma once

#include "IRendered.h"

struct Texture: public IRendered
{
	std::wstring Filename; // .dds only

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
	
	// TODO: use resource cache
	void Load(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
	{
		std::unique_ptr<u8[]> data;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;

		ThrowIfFailed(DirectX::LoadDDSTextureFromFile(
			device, Filename.c_str(), Resource.GetAddressOf(), data, subresources
		));
	}
};