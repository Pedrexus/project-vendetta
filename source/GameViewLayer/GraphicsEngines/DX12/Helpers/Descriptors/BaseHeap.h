#pragma once

#include "../../dx12pch.h"


/*
	6. GPU Descriptors

	a GPU resource is referenced through a descriptor object,
	which can be thought of as lightweight structure that describes the resource to the GPU.
	
	GPU resources are essentially generic chunks of memory. The descriptor reserves it for a purpose.
	
	In addition to identifying the resource data, descriptors describe the resource to the GPU:
	they tell Direct3D how the resource will be used (i.e., what stage of the pipeline you will bind it to)
	
	Descriptor types:
	1. CBV/SRV/UAV descriptors describe constant buffers, shader resources and unordered access view resources.
	2. Sampler descriptors describe sampler resources (used in texturing).
	3. RTV descriptors describe render target resources.
	4. DSV descriptors describe depth/stencil resources.
	
	-> descriptor heap = array of descriptors (block of GPU memory)
	-> descriptors should be created at initialization time.
	-> descriptor == view
*/
namespace Descriptor
{
	class BaseHeap
	{
	protected:
		inline void Create(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC* desc)
		{
			ThrowIfFailed(device->CreateDescriptorHeap(desc, IID_PPV_ARGS(heap.GetAddressOf())));
		}

	public:
		u32 descriptorSize = 0;
		ComPtr<ID3D12DescriptorHeap> heap = nullptr;

		// BaseHeap() = default;
		BaseHeap(ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_TYPE heapType)
		{
			descriptorSize = device->GetDescriptorHandleIncrementSize(heapType);
		}

		inline ID3D12DescriptorHeap* Get() { return heap.Get(); }

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
		}

		inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()
		{
			return CD3DX12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart());
		}
	};
}