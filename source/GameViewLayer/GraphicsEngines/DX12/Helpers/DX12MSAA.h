#pragma once

#include "../dx12pch.h"

/*
	MSAA = Multisample anti-aliasing

	this makes the RSV and DSV buffers 4x larger.

	Check 4X MSAA quality support for our back buffer format.
	All Direct3D 11 capable devices support 4X MSAA for all render
	target formats, so we only need to check quality support.
*/
namespace MSAA
{
	inline D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS Specify(
		DXGI_FORMAT backBufferFormat,
		u32 sampleCount,
		u32 numQualityLevels
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