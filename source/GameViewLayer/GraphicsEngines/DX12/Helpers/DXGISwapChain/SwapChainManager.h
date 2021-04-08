#pragma once

#include "../../dx12pch.h"
#include "../DX12Descriptor.h"

#include <GameLogicLayer/Game.h>

/*
	Double Buffering Swap Chain

	Front buffer: stores the image data currently being displayed on the monitor
	Back buffer: off-screen texture where the frame is being drawn
	Presenting: Swapping the roles of the back and front buffers after draw is done.

	The front and back buffer form a swap chain.
*/
class SwapChainManager
{
	static constexpr auto SwapChainBufferCount = 2;
	u8 currBackBuffer = 0;

	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12Resource> swapChainBuffers[SwapChainBufferCount];

	Descriptor::Heap rtvHeap;

public:
	static constexpr auto BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

protected:
	inline static DXGI_SWAP_CHAIN_DESC1 Specify(
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

	inline static DXGI_SWAP_CHAIN_FULLSCREEN_DESC SpecifyFullscreen(
		u32 refreshRate
	)
	{
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC sd = {};
		sd.RefreshRate.Numerator = refreshRate;
		sd.RefreshRate.Denominator = 1;
		sd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.Windowed = true;

		return sd;
	}

	void CreateSwapChainForWindow(IDXGIFactory* factory, const DXGI_SAMPLE_DESC& msaa, ID3D12CommandQueue* cmdQueue)
	{
		auto pfactory2 = static_cast<IDXGIFactory2*>(factory);
		auto ppSwapChain1 = (IDXGISwapChain1**)swapChain.GetAddressOf();

		auto window = Game::Get()->GetWindow();
		auto swapChainDesc = Specify(NULL, NULL, SwapChainBufferCount, BackBufferFormat, msaa);
		auto fsSwapChainDesc = SpecifyFullscreen(SCREEN_REFRESH_RATE);

		ThrowIfFailed(pfactory2->CreateSwapChainForHwnd(
			cmdQueue, window->GetMainWnd(),
			&swapChainDesc, &fsSwapChainDesc,
			nullptr, ppSwapChain1
		));
	}

	inline void SwapBuffers() { currBackBuffer = (currBackBuffer + 1) % SwapChainBufferCount; }

	ID3D12Resource* GetBuffer(u32 bufferIndex);

	void ResetBuffers();
	void ResizeBuffers(u32 width, u32 height);
	void CreateRenderTargetViewsForBuffers(ID3D12Device* device);

public:
	SwapChainManager(
		IDXGIFactory* factory,
		ID3D12Device* device,
		ID3D12CommandQueue* cmdQueue,
		DXGI_SAMPLE_DESC msaa
	);

	inline bool IsReady() { return swapChain; }

	inline ID3D12Resource* GetCurrentBackBuffer()
	{
		return swapChainBuffers[currBackBuffer].Get();
	}
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferCPUHandle()
	{
		return rtvHeap.GetCPUHandle(currBackBuffer);
	}
	inline void Present()
	{
		ThrowIfFailed(swapChain->Present(0, 0));
		SwapBuffers();
	}

	void Resize(ID3D12Device* device, u32 width, u32 height);

	DXGI_FRAME_STATISTICS GetFrameStatistics();
};