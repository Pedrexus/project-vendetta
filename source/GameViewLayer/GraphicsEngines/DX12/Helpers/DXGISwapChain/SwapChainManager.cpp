#include "SwapChainManager.h"

ID3D12Resource* SwapChainManager::GetBuffer(u32 bufferIndex)
{
	auto& buffer = swapChainBuffers[bufferIndex];
	ThrowIfFailed(swapChain->GetBuffer(bufferIndex, IID_PPV_ARGS(&buffer)));
	return buffer.Get();
}

void SwapChainManager::ResetBuffers()
{
	for (int i = 0; i < SwapChainBufferCount; ++i)
		swapChainBuffers[i].Reset();
	currBackBuffer = 0;
}

void SwapChainManager::ResizeBuffers(u32 width, u32 height)
{
	ThrowIfFailed(swapChain->ResizeBuffers(
		SwapChainBufferCount,
		width, height,
		BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)
	);
}

void SwapChainManager::CreateRenderTargetViewsForBuffers(ID3D12Device* device)
{
	auto rtvHeapHandle = rtvHeap.GetSwapChainRTVCreationHandle();
	for (auto i = 0; i < SwapChainBufferCount; i++)
	{
		device->CreateRenderTargetView(GetBuffer(i), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, rtvHeap.descriptorSize);
	}
}

SwapChainManager::SwapChainManager(IDXGIFactory* factory, ID3D12Device* device, ID3D12CommandQueue* cmdQueue, DXGI_SAMPLE_DESC msaa) :
	rtvHeap(device, SwapChainBufferCount)
{
	swapChain.Reset();
	CreateSwapChainForWindow(factory, msaa, cmdQueue);
}

void SwapChainManager::Resize(ID3D12Device* device, u32 width, u32 height)
{
	ResetBuffers();
	ResizeBuffers(width, height);
	CreateRenderTargetViewsForBuffers(device);
}

DXGI_FRAME_STATISTICS SwapChainManager::GetFrameStatistics()
{
	DXGI_FRAME_STATISTICS stats = {};
	swapChain->GetFrameStatistics(&stats);
	return stats;
}
