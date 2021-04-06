#pragma once

#include "DX12DXGI.h"

inline bool HadSuccessCreatingDevice(HRESULT createDeviceResult, IDXGIAdapter4* adapter)
{
	DXGI_ADAPTER_DESC3 adapterDesc;
	adapter->GetDesc3(&adapterDesc);

	if (SUCCEEDED(createDeviceResult))
	{
		LOG("Graphics", "D3D12Device created with " + Convert::wide2str(adapterDesc.Description));
		return true;
	}
	else
	{
		LOG_WARNING("Failure at D3D12CreateDevice with " + Convert::wide2str(adapterDesc.Description));
		return false;
	}
}

inline HRESULT CreateDeviceFromAdapters(Display::AdapterVector adapterVector, ID3D12Device** device)
{
	HRESULT createDeviceResult = E_FAIL;
	for (auto& adapter : adapterVector)
	{
		createDeviceResult = D3D12CreateDevice(adapter, DXD12_MINIMUM_FEATURE_LEVEL, IID_PPV_ARGS(device));
		if (HadSuccessCreatingDevice(createDeviceResult, adapter))
			break;
	}

	return createDeviceResult;
}

inline void CreateWARPDevice(IDXGIFactory4* factory, ID3D12Device** device)
{
	ComPtr<IDXGIAdapter> pWarpAdapter;
	ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
	ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), DXD12_MINIMUM_FEATURE_LEVEL, IID_PPV_ARGS(device)));
}

// A device represents the display adapter (graphics card). Direct3D 12 devices are singletons per adapter.
inline void CreateHardwareDevice(Display::AdapterVector adapterVector, IDXGIFactory6* factory, ID3D12Device** device)
{
	auto createDeviceResult = CreateDeviceFromAdapters(adapterVector, device);

	// Fallback to WARP device.
	if (FAILED(createDeviceResult))
	{
		LOG_WARNING("Failure at D3D12CreateDevice with Highest Performance Adapter. Falling back to WARP device.");
		CreateWARPDevice(factory, device);
	}
}
