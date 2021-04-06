#pragma once

#include "dx12pch.h"


namespace MSAA
{
	inline D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS Create(
		DXGI_FORMAT backBufferFormat, 
		u32 sampleCount = 4, 
		u32 numQualityLevels = 0
	)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels = {};
		qualityLevels.Format = backBufferFormat;
		qualityLevels.SampleCount = sampleCount;
		qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		qualityLevels.NumQualityLevels = numQualityLevels;

		return qualityLevels;
	}
}



inline D3D12_VIEWPORT CreateViewport(u32 width, u32 height)
{
	D3D12_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	return vp;
}

inline D3D12_RECT CreateScissorRectangle(i32 left, i32 top, i32 right, i32 bottom)
{
	return { left, top, right, bottom };
}

