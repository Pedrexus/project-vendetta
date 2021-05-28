#pragma once

#include "BaseHeap.h"

namespace Descriptor
{
	/* The descriptor heap for the combination of constant-buffer, shader-resource, and unordered-access views. */
	namespace GeneralResource
	{
		static constexpr auto Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		class Heap : public Descriptor::BaseHeap
		{

			inline D3D12_DESCRIPTOR_HEAP_DESC Specify(u32 numDescriptors)
			{
				D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
				cbvHeapDesc.NumDescriptors = numDescriptors;
				cbvHeapDesc.Type = GeneralResource::Type;
				cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				cbvHeapDesc.NodeMask = 0;
				return cbvHeapDesc;
			}

		public:
			void Create(ID3D12Device* device, u32 numDescriptors)
			{
				const auto desc = Specify(numDescriptors);
				BaseHeap::Create(device, &desc, GeneralResource::Type);
			}

			void AddTexture2D(ID3D12Device* device, ID3D12Resource* texResource)
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = texResource->GetDesc().Format;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = texResource->GetDesc().MipLevels;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				device->CreateShaderResourceView(texResource, &srvDesc, GetCPUHandle());
			};
		};
	};
};