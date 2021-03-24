#pragma once

#include "dx12pch.h"


namespace MSAA
{
	inline D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS Create(
		DXGI_FORMAT backBufferFormat, 
		u32 sampleCount = 4, 
		u32 numQualityLevels = 0
	)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels = {};
		qualityLevels.Format = backBufferFormat;
		qualityLevels.SampleCount = sampleCount;
		qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		qualityLevels.NumQualityLevels = numQualityLevels;

		return qualityLevels;
	}
}

namespace Command
{
	inline D3D12_COMMAND_QUEUE_DESC Queue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		return queueDesc;
	}
}

namespace SwapChain
{
	inline DXGI_SWAP_CHAIN_DESC1 Create(
		u32 width,
		u32 height,
		DXGI_FORMAT backBufferFormat,
		u32 MSAACount,
		u32 MSAAQuality,
		u32 swapChainBufferCount
	)
	{
		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = width;
		sd.Height = height;
		sd.Format = backBufferFormat;
		sd.Stereo = false;
		sd.SampleDesc.Count = MSAACount;
		sd.SampleDesc.Quality = MSAAQuality;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = swapChainBufferCount;
		sd.Scaling = DXGI_SCALING_NONE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		return sd;
	}

	inline DXGI_SWAP_CHAIN_FULLSCREEN_DESC Fullscreen(u32 refreshRate)
	{
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC sd = {};
		sd.RefreshRate.Numerator = refreshRate;
		sd.RefreshRate.Denominator = 1;
		sd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.Windowed = true;

		return sd;
	}
}

namespace Descriptor
{
	namespace RenderTarget
	{
		inline D3D12_DESCRIPTOR_HEAP_DESC Heap(u32 swapChainBufferCount)
		{
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
		inline D3D12_DESCRIPTOR_HEAP_DESC Heap()
		{
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
			dsvHeapDesc.NumDescriptors = 1;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			dsvHeapDesc.NodeMask = 0;
			return dsvHeapDesc;
		}

		inline D3D12_RESOURCE_DESC Buffer(u32 width, u32 height, DXGI_FORMAT format)
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

		inline D3D12_CLEAR_VALUE ClearValue(DXGI_FORMAT format)
		{
			D3D12_CLEAR_VALUE optClear = {};
			optClear.Format = format;
			optClear.DepthStencil.Depth = 1.0f;
			optClear.DepthStencil.Stencil = 0;
			return optClear;
		}

		inline D3D12_DEPTH_STENCIL_VIEW_DESC View(DXGI_FORMAT format, u32 mipSlice2D = 0)
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Format = format;
			dsvDesc.Texture2D.MipSlice = mipSlice2D;
			return dsvDesc;
		}
	}
}

inline D3D12_VIEWPORT CreateViewport(u32 width, u32 height)
{
	D3D12_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	return vp;
}

inline D3D12_RECT CreateScissorRectangle(i32 left, i32 top, i32 right, i32 bottom)
{
	return { left, top, right, bottom };
}

namespace Display
{
	std::vector<IDXGIOutput*> AdapterOutputs(IDXGIAdapter4* adapter)
	{
		UINT i = 0;
		IDXGIOutput* output = nullptr;
		std::vector<IDXGIOutput*> outputList;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
		{
			outputList.push_back(output);
			++i;
		}

		return outputList;
	}

	std::vector<DXGI_MODE_DESC1> OutputModes(IDXGIOutput1* output, DXGI_FORMAT backBufferFormat)
	{
		UINT count = 0;
		UINT flags = 0;

		// Call with nullptr to get list count.
		output->GetDisplayModeList1(backBufferFormat, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC1> modeList(count);
		output->GetDisplayModeList1(backBufferFormat, flags, &count, &modeList[0]);

		return modeList;
	}

}