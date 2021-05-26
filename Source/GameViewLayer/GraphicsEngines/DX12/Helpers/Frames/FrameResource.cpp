#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, u32 objectCount, u32 materialCount, u8 index) :
	_PassCB(device, 1),
	_ObjectCB(device, objectCount),
	_MaterialCB(device, objectCount),
	_cbvHeap(device, objectCount + 1), // TODO: using views now, not needed anymore
	Index(index)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_CmdListAlloc)));
	CreateConstantBufferViews(device, objectCount);
}

void FrameResource::UpdateObjectConstantBuffers(u32 objIndex, const ObjectConstants& objConstants)
{
	_ObjectCB.Upload(objIndex, objConstants);
}

void FrameResource::UpdateMaterialConstantBuffers(u32 index, const MaterialConstants& matConstants)
{
	_MaterialCB.Upload(index, matConstants);
}

void FrameResource::UpdateMainPassConstantBuffers(const RenderPassConstants& passConstants)
{
	_PassCB.Upload(&passConstants);
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
