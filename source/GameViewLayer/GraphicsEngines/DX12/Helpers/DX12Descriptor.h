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
	namespace RenderTarget
	{
		static constexpr auto Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		inline D3D12_DESCRIPTOR_HEAP_DESC SpecifyHeap(u32 swapChainBufferCount)
		{
			assert(swapChainBufferCount > 0);

			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = swapChainBufferCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			rtvHeapDesc.NodeMask = 0;

			return rtvHeapDesc;
		}
	}

	namespace DepthStencil
	{
		static constexpr auto Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

		inline D3D12_DESCRIPTOR_HEAP_DESC SpecifyHeap()
		{
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
			dsvHeapDesc.NumDescriptors = 1;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			dsvHeapDesc.NodeMask = 0;
			return dsvHeapDesc;
		}

		inline D3D12_RESOURCE_DESC SpecifyBuffer(u32 width, u32 height, DXGI_FORMAT format)
		{
			D3D12_RESOURCE_DESC depthStencilDesc = {};
			depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  // depth buffer must be 2D texture (dim = 3)
			depthStencilDesc.Alignment = 0;
			depthStencilDesc.Width = width;
			depthStencilDesc.Height = height;
			depthStencilDesc.DepthOrArraySize = 1;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.SampleDesc.Count = 1; // MSAA must match the settings used for the render target.
			depthStencilDesc.SampleDesc.Quality = 0;
			depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			depthStencilDesc.Format = format;
			return depthStencilDesc;
		}

		inline D3D12_CLEAR_VALUE SpecifyClearValue(DXGI_FORMAT format)
		{
			D3D12_CLEAR_VALUE optClear = {};
			optClear.Format = format;
			optClear.DepthStencil.Depth = 1.0f;
			optClear.DepthStencil.Stencil = 0;
			return optClear;
		}

		inline D3D12_DEPTH_STENCIL_VIEW_DESC SpecifyView(DXGI_FORMAT format, u32 mipSlice2D = 0)
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Format = format;
			dsvDesc.Texture2D.MipSlice = mipSlice2D;
			return dsvDesc;
		}
	}

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

		// Each RTV buffer has its own handle
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle(u32 currentBackBuffer) const
		{
			assert(currentBackBuffer <= UINT8_MAX);
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart(), currentBackBuffer, descriptorSize);
		}

		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTVCreationHandle()
		{
			assert(heapType == RenderTarget::Type);
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(u32 currentBackBuffer = -1)
		{
			switch (heapType)
			{
				case RenderTarget::Type: 
					return GetCurrentBackBufferHandle(currentBackBuffer);
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
				case RenderTarget::Type:
				case DepthStencil::Type:
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
				case RenderTarget::Type: return RenderTarget::SpecifyHeap(swapChainBufferCount);
				case DepthStencil::Type: return DepthStencil::SpecifyHeap();
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