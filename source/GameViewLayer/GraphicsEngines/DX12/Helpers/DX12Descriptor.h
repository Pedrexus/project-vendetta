#pragma once

#include "../dx12pch.h"

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

	
	namespace ConstantBuffer
	{
		static constexpr auto Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		inline D3D12_DESCRIPTOR_HEAP_DESC SpecifyHeap()
		{
			D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
			cbvHeapDesc.NumDescriptors = 1;
			cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // this descriptor is going to be accessed by a shader
			cbvHeapDesc.NodeMask = 0;
			return cbvHeapDesc;
		}
	}

	class Heap
	{
	public:
		ComPtr<ID3D12DescriptorHeap> heap;
		
		u32 descriptorSize = 0;
		D3D12_DESCRIPTOR_HEAP_TYPE heapType;

		Heap() : descriptorSize(NULL) {};
		Heap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, u32 swapChainBufferCount = NULL) :
			heapType(heapType)
		{
			descriptorSize = device->GetDescriptorHandleIncrementSize(heapType);
			Create(device, swapChainBufferCount);
		}

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTVCreationHandle()
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(u32 currentBackBuffer = -1)
		{
			switch (heapType)
			{
				case DepthStencil::Type:
				case ConstantBuffer::Type: 
					return heap->GetCPUDescriptorHandleForHeapStart();
				default: 
					return {};
			}
		}

		inline D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(u32 currentBackBuffer = -1)
		{
			switch (heapType)
			{
				case ConstantBuffer::Type:
					return heap->GetGPUDescriptorHandleForHeapStart();
				default:
					return {};
			}
		}

		inline D3D12_DESCRIPTOR_HEAP_DESC Specify(u32 swapChainBufferCount = NULL)
		{
			switch (heapType)
			{
				case ConstantBuffer::Type: return ConstantBuffer::SpecifyHeap();
				default:
					return {};
			}
		}

		inline void Create(ID3D12Device* device, u32 swapChainBufferCount = NULL)
		{
			auto desc = Specify(swapChainBufferCount);
			ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap.GetAddressOf())));
		}

	};
}