#pragma once

#pragma once

#include <dx12pch.h>

#include <GameViewLayer/GraphicsElements/Material.h>
#include <GameViewLayer/GraphicsElements/Object.h>

#include "../Buffers/ConstantBuffer.h"

// TODO: remove this constant definitions from here
struct RenderPassConstants
{
	XMMATRIX ViewProj = XMMatrixIdentity();

	XMFLOAT3 EyePosition = {};
	f32 Time = .0f;

	f32 dt = .0f;
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
	ConstantBuffer<MaterialConstants> _MaterialCB;

	Descriptor::ConstantBuffer::Heap _cbvHeap;

protected:
	u64 Fence = 0;

public:
	const u8 Index;

	FrameResource(ID3D12Device* device, u32 objectCount, u32 materialCount, u8 index);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;

	void UpdateMainPassConstantBuffers(const RenderPassConstants& passConstants);
	void UpdateObjectConstantBuffers(u32 objIndex, const ObjectConstants& objConstants);
	void UpdateMaterialConstantBuffers(u32 index, const MaterialConstants& matCtes);

	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

	void CreateConstantBufferViews(ID3D12Device* device, u32 objectCount);
};