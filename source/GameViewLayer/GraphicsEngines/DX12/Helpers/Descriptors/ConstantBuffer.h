#pragma once

#include "BaseHeap.h"

namespace Descriptor
{
	namespace ConstantBuffer
	{
		static constexpr auto Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		class Heap : public Descriptor::BaseHeap
		{
			inline D3D12_DESCRIPTOR_HEAP_DESC Specify(u32 numDescriptors)
			{
				D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
				cbvHeapDesc.NumDescriptors = numDescriptors;
				cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // this descriptor is going to be accessed by a shader
				cbvHeapDesc.NodeMask = 0;
				return cbvHeapDesc;
			}

		public:
			Heap(ID3D12Device* device, u32 numDescriptors) : BaseHeap(device, ConstantBuffer::Type)
			{
				const auto cbvDesc = Specify(numDescriptors);
				Create(device, &cbvDesc);
			}
		};

	}
}