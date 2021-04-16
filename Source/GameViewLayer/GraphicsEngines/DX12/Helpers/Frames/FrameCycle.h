#pragma once

#include <const.h>

#include "FrameResource.h"
#include "../D3D12Fence/FenceManager.h"
#include "../Descriptors/ConstantBuffer.h"

class FrameCycle
{
	std::array<std::unique_ptr<FrameResource>, NUMBER_FRAME_RESOURCES> _FrameResources;
	u8 _CurrFrameResourceIndex = 0;

	FenceManager _Fence;

	// Descriptor::ConstantBuffer::Heap _cbvHeap; _cbvHeap(device, NUMBER_FRAME_RESOURCES* (objectCount + 1)),

public:
	FrameCycle(ID3D12Device* device, u32 objectCount) :
		_Fence(device)
	{
		for (auto i = 0; i < NUMBER_FRAME_RESOURCES; i++)
			_FrameResources[i] = std::make_unique<FrameResource>(device, objectCount);
	}

	inline void Advance()
	{
		_CurrFrameResourceIndex = (_CurrFrameResourceIndex + 1) % NUMBER_FRAME_RESOURCES;
	}

	inline FrameResource* GetCurrentFrameResource()
	{
		return _FrameResources[_CurrFrameResourceIndex].get();
	}

	inline void Sync(ID3D12CommandQueue* commandQueue)
	{
		// Cycle through the circular frame resource array.
		Advance();

		// FIXME: first 3 frames are going to be slower
		auto frameFence = GetCurrentFrameResource()->Fence;
		if (frameFence != 0 && !_Fence.IsSynchronized())
			_Fence.WaitForGPU();

		auto currFence = _Fence.Advance();
		GetCurrentFrameResource()->Fence = currFence;
		ThrowIfFailed(commandQueue->Signal(_Fence.Get(), currFence));
	}

	inline void Flush(ID3D12CommandQueue* commandQueue)
	{
		ThrowIfFailed(commandQueue->Signal(_Fence.Get(), _Fence.Advance()));
		if (!_Fence.IsSynchronized())
			_Fence.WaitForGPU();
	}

	inline void UpdateCurrentFrameConstantBuffers(ObjectConstants objConstants, RenderPassConstants passConstants)
	{
		auto currFrame = GetCurrentFrameResource();
		currFrame->UpdateObjectConstantBuffers(objConstants);
		currFrame->UpdateMainPassConstantBuffers(passConstants);
	}

	/*
	inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetCurrentFrameMainPassGPUHandle(u64 numRenderItems)
	{
		// Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
		auto passCbvOffset = numRenderItems * NUMBER_FRAME_RESOURCES;
		auto index = passCbvOffset + _CurrFrameResourceIndex;
		auto gpuHandle = _cbvHeap.GetGPUHandle();
		gpuHandle.Offset(index, _cbvHeap.descriptorSize);
		return gpuHandle;
	}

	inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetCurrentFrameRenderItemGPUHandle(u64 numRenderItems, u32 objCBIndex)
	{
		auto index = _CurrFrameResourceIndex * numRenderItems + objCBIndex;
		auto gpuHandle = _cbvHeap.GetGPUHandle();
		gpuHandle.Offset(index, _cbvHeap.descriptorSize);
		return gpuHandle;
	}

	inline ID3D12DescriptorHeap* GetConstantBufferDescriptorHeap()
	{
		return _cbvHeap.Get();
	}
	

	void BuildConstantBufferViews(ID3D12Device* device, u32 objectCount)
	{
		// TODO: auto cpuHandle = _cbvHeap.GetCPUHandle(); -> should be simpler if the cpuHandle stores the index

		// Need a CBV descriptor for each object for each frame resource.
		for (u64 i = 0; i < NUMBER_FRAME_RESOURCES; i++)
		{
			auto objCBByteSize = _FrameResources[i]->_ObjectCB.CalcBufferByteSize();
			auto objCBResource = _FrameResources[i]->_ObjectCB.GetResource();

			for (u64 j = 0; j < objectCount; j++)
			{
				auto cbAddress = objCBResource->GetGPUVirtualAddress(); // TODO: move it out of loop

				// Offset to the ith object constant buffer in the buffer.
				cbAddress += j * objCBByteSize;

				// Offset to the object cbv in the descriptor heap.
				auto heapIndex = i * objectCount + j;
				auto cpuHandle = _cbvHeap.GetCPUHandle();
				cpuHandle.Offset(heapIndex, _cbvHeap.descriptorSize);

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
				cbvDesc.BufferLocation = cbAddress;
				cbvDesc.SizeInBytes = objCBByteSize;

				device->CreateConstantBufferView(&cbvDesc, cpuHandle);
			}
		}

		auto passCbvOffset = objectCount * NUMBER_FRAME_RESOURCES;

		// Last three descriptors are the pass CBVs for each frame resource.
		for (u64 i = 0; i < NUMBER_FRAME_RESOURCES; i++)
		{
			auto passCBByteSize = _FrameResources[i]->_PassCB.CalcBufferByteSize();
			ID3D12Resource* passCBResource = _FrameResources[i]->_PassCB.GetResource();

			auto cbAddress = passCBResource->GetGPUVirtualAddress();

			// Offset to the pass cbv in the descriptor heap.
			auto heapIndex = passCbvOffset + i;
			auto cpuHandle = _cbvHeap.GetCPUHandle();
			cpuHandle.Offset(heapIndex, _cbvHeap.descriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = passCBByteSize;

			device->CreateConstantBufferView(&cbvDesc, cpuHandle);
		}
	}

	inline ID3D12CommandAllocator* ResetAndGetCommandListAllocator()
	{
		auto frameCmdListAlloc = GetCurrentFrameResource()->_CmdListAlloc.Get();
		ThrowIfFailed(frameCmdListAlloc->Reset());
		return frameCmdListAlloc;
	}
	*/
};