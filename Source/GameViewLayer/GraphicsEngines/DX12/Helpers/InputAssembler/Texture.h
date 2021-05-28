#pragma once

#include <GameViewLayer/GraphicsElements/Texture.h>

struct RenderTexture : public Texture
{
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;

	// TODO: use resource cache
	void Load(ID3D12Device* device)
	{
		std::unique_ptr<u8[]> data;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;

		ThrowIfFailed(DirectX::LoadDDSTextureFromFile(
			device, Filename.c_str(), Resource.GetAddressOf(), data, subresources
		));
	}
};
