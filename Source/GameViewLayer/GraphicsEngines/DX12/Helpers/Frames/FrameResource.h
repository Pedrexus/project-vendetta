#pragma once

#pragma once

#include "../../dx12pch.h"
#include "../Buffers/ConstantBuffer.h"

struct ObjectConstants
{
	XMMATRIX World = XMMatrixIdentity();
};

struct RenderPassConstants
{
	XMMATRIX ViewProj = XMMatrixIdentity();
	f32 Time = .0f;
	f64 DeltaTime = .0f;
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
	ConstantBuffer<ObjectConstants> _ObjectCB;
	ConstantBuffer<RenderPassConstants> _PassCB;

protected:
	u64 Fence = 0;

public:
	FrameResource(ID3D12Device* device, u32 objectCount) :
		_PassCB(device, 1),
		_ObjectCB(device, objectCount)
	{
		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_CmdListAlloc)));
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

	std::array<ID3D12DescriptorHeap*, 2> GetDescriptorHeaps() const
	{
		return { _ObjectCB.cbvHeap.heap.Get(), _PassCB.cbvHeap.heap.Get() };
	}

	ID3D12DescriptorHeap* GetMainPassDescriptorHeap()
	{
		return _PassCB.cbvHeap.heap.Get();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetMainPassGPUHandle()
	{
		return _PassCB.cbvHeap.GetGPUHandle();
	}


};