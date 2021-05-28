#pragma once

#include "BaseHeap.h"

namespace Descriptor
{
	namespace DepthStencil
	{
		static constexpr auto Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

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

		class Heap : public BaseHeap
		{
			inline D3D12_DESCRIPTOR_HEAP_DESC Specify()
			{
				D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
				dsvHeapDesc.NumDescriptors = 1;
				dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
				dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				dsvHeapDesc.NodeMask = 0;
				return dsvHeapDesc;
			}

		public:
			Heap(ID3D12Device* device)
			{
				const auto dsvDesc = Specify();
				Create(device, &dsvDesc, DepthStencil::Type);
			}
		};

	}
}