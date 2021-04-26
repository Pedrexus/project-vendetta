#include "DepthStencilManager.h"

void DepthStencilManager::CreateResource(const u32& width, const u32& height, ID3D12Device* device)
{
	auto resourceDesc = Specify(width, height);
	auto optClear = SpecifyClearValue();
	auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	depthStencilBuffer.Reset();
	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeap,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(&depthStencilBuffer))
	);
}

void DepthStencilManager::CreateView(ID3D12Device* device)
{
	auto dsvDesc = SpecifyView();
	device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, GetCPUHandle());
}

void DepthStencilManager::Resize(ID3D12Device* device, u32 width, u32 height)
{
	CreateResource(width, height, device);
	CreateView(device);
}

// Transition the resource from its initial state to be used as a depth buffer.

CD3DX12_RESOURCE_BARRIER* DepthStencilManager::GetWriteTransition()
{
	Transition = CD3DX12_RESOURCE_BARRIER::Transition(
		depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	return &Transition;
}
