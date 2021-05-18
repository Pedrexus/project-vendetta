#include "FrameCycle.h"
#include <Helpers/Settings/Settings.h>

FrameCycle::FrameCycle(ID3D12Device* device, u32 objectCount) :
	_Fence(device)
{
	auto numFrameResources = Settings::GetInt("graphics-frame-resources");
	_FrameResources.resize(numFrameResources);
	for (auto i = 0; i < numFrameResources; i++)
		_FrameResources[i] = std::make_unique<FrameResource>(device, objectCount, i);
}

void FrameCycle::SyncFrameResource(u64 frameFence)
{
	if (frameFence != 0 && !_Fence.IsSynchronized(frameFence))
		_Fence.WaitForGPU(frameFence);
}

// We check the GPU is done with the frame with a signal

void FrameCycle::SignalCurrentFrame(ID3D12CommandQueue* commandQueue)
{
	auto currFence = _Fence.Advance();
	GetCurrentFrameResource()->Fence = currFence;
	ThrowIfFailed(commandQueue->Signal(_Fence.Get(), currFence));
}

void FrameCycle::Advance()
{
	_CurrFrameResourceIndex = (_CurrFrameResourceIndex + 1) % Settings::GetInt("graphics-frame-resources");
}

FrameResource* FrameCycle::GetCurrentFrameResource()
{
	return _FrameResources[_CurrFrameResourceIndex].get();
}

ID3D12CommandAllocator* FrameCycle::GetCurrentFrameAllocatorWhenAvailable()
{
	auto currFrame = GetCurrentFrameResource();
	SyncFrameResource(currFrame->Fence);
	return currFrame->_CmdListAlloc.Get();
}

void FrameCycle::Flush(ID3D12CommandQueue* commandQueue)
{
	ThrowIfFailed(commandQueue->Signal(_Fence.Get(), _Fence.Advance()));
	if (!_Fence.IsSynchronized())
		_Fence.WaitForGPU();
}