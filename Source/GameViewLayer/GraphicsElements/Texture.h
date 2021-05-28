#pragma once

#include "IRendered.h"

struct Texture
{
	std::string Name;
	std::wstring Filename; // .dds only

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};