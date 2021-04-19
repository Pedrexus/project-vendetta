#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, u32 objectCount) :
	_PassCB(device, 1),
	_ObjectCB(device, objectCount),
	_cbvHeap(device, objectCount + 1)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_CmdListAlloc)));
	CreateConstantBufferViews(device, objectCount);
}

void FrameResource::UpdateObjectConstantBuffers(ObjectConstants objConstants)
{
	_ObjectCB.CopyToCPUBuffer(0, objConstants);
}

void FrameResource::UpdateMainPassConstantBuffers(RenderPassConstants passConstants)
{
	_PassCB.CopyToCPUBuffer(0, passConstants);
}

ID3D12DescriptorHeap* FrameResource::GetDescriptorHeap() const
{
	return _cbvHeap.heap.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE FrameResource::GetGPUHandle()
{
	return _cbvHeap.GetGPUHandle();
}

void FrameResource::CreateConstantBufferViews(ID3D12Device* device, u32 objectCount)
{
	// 1. Pass views
	auto cbvPassDesc = _PassCB.SpecifyConstantBufferView();
	auto cpuHandle = _cbvHeap.GetCPUHandle();
	device->CreateConstantBufferView(&cbvPassDesc, cpuHandle);

	// only one view is created for RenderPassContants
	u8 passOffset = 1;

	// 2. Object views
	for (u8 i = 0; i < objectCount; i++)
	{
		auto cbvDesc = _ObjectCB.SpecifyConstantBufferView(i);

		auto cpuHandle = _cbvHeap.GetCPUHandle();
		cpuHandle.Offset(passOffset + i, _cbvHeap.descriptorSize);

		device->CreateConstantBufferView(&cbvDesc, cpuHandle);
	}
}