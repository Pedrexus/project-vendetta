#pragma once

#include <const.h>

#include "FrameResource.h"
#include "../D3D12Fence/FenceManager.h"

class FrameCycle
{
	std::array<std::unique_ptr<FrameResource>, NUMBER_FRAME_RESOURCES> _FrameResources;
	u8 _CurrFrameResourceIndex = 0;

	FenceManager _Fence;

public:
	FrameCycle(ID3D12Device* device, u32 objectCount);

protected:
	void SyncFrameResource(u64 frameFence);

public:
	// Cycle through the circular frame resource array
	void Advance();

	FrameResource* GetCurrentFrameResource();

	// We check the GPU is done with the frame with a signal
	void SignalCurrentFrame(ID3D12CommandQueue* commandQueue);

	ID3D12CommandAllocator* GetCurrentFrameAllocatorWhenAvailable();

	void Flush(ID3D12CommandQueue* commandQueue);
};