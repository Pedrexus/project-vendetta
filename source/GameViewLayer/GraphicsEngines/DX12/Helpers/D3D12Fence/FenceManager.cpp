#include "FenceManager.h"

FenceManager::FenceManager(ID3D12Device* device)
{
	assert(currentFence == 0);
	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}