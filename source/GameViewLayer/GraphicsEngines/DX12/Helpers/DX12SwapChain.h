#pragma once

#include "../dx12pch.h"

/*
	Front buffer: stores the image data currently being displayed on the monitor
	Back buffer: off-screen texture where the frame is being drawn
	Presenting: Swapping the roles of the back and front buffers after draw is done.

	The front and back buffer form a swap chain.
*/
namespace SwapChain
{
	inline DXGI_SWAP_CHAIN_DESC1 Specify(
		u32 width,
		u32 height,
		u32 swapChainBufferCount,
		DXGI_FORMAT backBufferFormat,
		DXGI_SAMPLE_DESC msaa
	)
	{
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = width;
		sd.Height = height;
		sd.Format = backBufferFormat;
		sd.Stereo = false;
		sd.SampleDesc = msaa;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = swapChainBufferCount;
		sd.Scaling = DXGI_SCALING_NONE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		return sd;
	}

	inline DXGI_SWAP_CHAIN_FULLSCREEN_DESC SpecifyFullscreen(u32 refreshRate)
	{
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC sd = {};
		sd.RefreshRate.Numerator = refreshRate;
		sd.RefreshRate.Denominator = 1;
		sd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.Windowed = true;

		return sd;
	}

	inline ComPtr<IDXGISwapChain1> Create()
	{
		auto swapChain = ComPtr<IDXGISwapChain1>();

	}
}