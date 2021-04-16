#pragma once

#pragma once

#include "../../dx12pch.h"
#include "../Buffers/ConstantBuffer.h"

struct RenderPassConstants
{
	XMMATRIX ViewProj = XMMatrixIdentity();
	f32 Time = .0f;
	f64 DeltaTime = .0f;
};

struct ObjectConstants
{
	XMMATRIX World = XMMatrixIdentity();
};

// Stores the resources needed for the CPU to build the command lists
// for a frame.  
struct FrameResource
{
	friend class FrameCycle;

	// We cannot reset the allocator until the GPU is done processing the commands.
	// So each frame needs their own allocator.
	ComPtr<ID3D12CommandAllocator> _CmdListAlloc;

	// We cannot update a cbuffer until the GPU is done processing the commands
	// that reference it. So each frame needs their own cbuffers.
	ConstantBuffer<RenderPassConstants> _PassCB;
	ConstantBuffer<ObjectConstants> _ObjectCB;

	Descriptor::ConstantBuffer::Heap _cbvHeap;

protected:
	u64 Fence = 0;

public:
	FrameResource(ID3D12Device* device, u32 objectCount) :
		_PassCB(device, 1),
		_ObjectCB(device, objectCount),
		_cbvHeap(device, objectCount + 1)
	{
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_CmdListAlloc)));
		CreateConstantBufferViews(device, objectCount);
	}

	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;

	void UpdateObjectConstantBuffers(ObjectConstants objConstants)
	{
		_ObjectCB.CopyToCPUBuffer(0, objConstants);
	}

	void UpdateMainPassConstantBuffers(RenderPassConstants passConstants)
	{
		_PassCB.CopyToCPUBuffer(0, passConstants);
	}

	ID3D12DescriptorHeap* GetDescriptorHeap() const
	{
		return _cbvHeap.heap.Get();
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()
	{
		return _cbvHeap.GetGPUHandle();
	}

	void CreateConstantBufferViews(ID3D12Device* device, u32 objectCount)
	{
		// 1. Pass views
		auto cbvPassDesc = _PassCB.SpecifyConstantBufferView();
		auto cpuHandle = _cbvHeap.GetCPUHandle();
		device->CreateConstantBufferView(&cbvPassDesc, cpuHandle);

		auto passOffset = 1;

		// 2. Object views
		for (u64 i = 0; i < objectCount; i++)
		{
			auto cbvDesc = _ObjectCB.SpecifyConstantBufferView(i);

			auto cpuHandle = _cbvHeap.GetCPUHandle();
			cpuHandle.Offset(passOffset + i, _cbvHeap.descriptorSize);
			
			device->CreateConstantBufferView(&cbvDesc, cpuHandle);
		}
	}

};