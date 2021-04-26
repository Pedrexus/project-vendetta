#pragma once

#include "../../dx12pch.h"
#include "../Descriptors/DepthStencil.h"

class DepthStencilManager
{
	ComPtr<ID3D12Resource> depthStencilBuffer;
	Descriptor::DepthStencil::Heap dsvHeap;
	CD3DX12_RESOURCE_BARRIER Transition;

public:
	DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

protected:
	inline D3D12_RESOURCE_DESC Specify(u32 width, u32 height)
	{
		D3D12_RESOURCE_DESC depthStencilDesc = {};
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // allows creation of two views from the same resource
		depthStencilDesc.SampleDesc = { 1, 0 };
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		return depthStencilDesc;
	}

	inline D3D12_CLEAR_VALUE SpecifyClearValue()
	{
		static D3D12_CLEAR_VALUE optClear = {};
		optClear.Format = depthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		return optClear;
	}

	inline D3D12_DEPTH_STENCIL_VIEW_DESC SpecifyView()
	{
		static D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = depthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		return dsvDesc;
	}

	void CreateResource(const u32& width, const u32& height, ID3D12Device* device);
	void CreateView(ID3D12Device* device);

public:
	DepthStencilManager(ID3D12Device* device) : dsvHeap(device) {}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()
	{
		return dsvHeap.GetCPUHandle();
	}

	void Resize(ID3D12Device* device, u32 width, u32 height);

	// Transition the resource from its initial state to be used as a depth buffer.
	CD3DX12_RESOURCE_BARRIER* GetWriteTransition();
};

