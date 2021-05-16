#pragma once

#pragma once

#include "../../dx12pch.h"
#include "../Buffers/ConstantBuffer.h"
#include "../Buffers/InputAssembler/DynamicMeshBuffer.h"

struct RenderPassConstants
{
	XMMATRIX ViewProj = XMMatrixIdentity();
	f32 Time = .0f;
	f64 DeltaTime = .0f;
};

struct ObjectConstants
{
	XMFLOAT4X4 World;
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
	const u8 Index;

	FrameResource(ID3D12Device* device, u32 objectCount, u8 index);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;

	void UpdateObjectConstantBuffers(u32 objIndex, ObjectConstants objConstants);
	void UpdateMainPassConstantBuffers(RenderPassConstants passConstants);

	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

	void CreateConstantBufferViews(ID3D12Device* device, u32 objectCount);
};