#pragma once

#include "BaseHeap.h"

namespace Descriptor
{
	namespace RenderTarget
	{
		static constexpr auto Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		class Heap : public Descriptor::BaseHeap
		{
			inline D3D12_DESCRIPTOR_HEAP_DESC Specify(u32 swapChainBufferCount)
			{
				assert(swapChainBufferCount > 0);

				D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
				rtvHeapDesc.NumDescriptors = swapChainBufferCount;
				rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				rtvHeapDesc.NodeMask = 0;

				return rtvHeapDesc;
			}
		public:
			Heap(ID3D12Device* device, u32 swapChainBufferCount)
			{
				const auto rtvDesc = Specify(swapChainBufferCount);
				Create(device, &rtvDesc, RenderTarget::Type);
			}

			// Each RTV buffer has its own handle
			inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(u32 currentBackBuffer) const
			{
				return CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart(), currentBackBuffer, descriptorSize);
			}

			inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTVCreationHandle() const
			{
				return CD3DX12_CPU_DESCRIPTOR_HANDLE(heap->GetCPUDescriptorHandleForHeapStart());
			}
		};

	}
}