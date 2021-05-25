#pragma once

#include <dx12pch.h>

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

	inline DXGI_SAMPLE_DESC Check(
		ID3D12Device* device,
		DXGI_FORMAT backBufferFormat,
		u32 sampleCount,
		u32 numQualityLevels
	)
	{
		auto msaa = Specify(backBufferFormat, sampleCount, numQualityLevels);
		ThrowIfFailed(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaa, sizeof(msaa)));

		if (msaa.NumQualityLevels < 0)
			LOG_ERROR("Unexpected MSAA quality level.");

		return { msaa.SampleCount, msaa.NumQualityLevels - 1 };
	}
}