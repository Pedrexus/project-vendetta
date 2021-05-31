#pragma once

#include <types.h>
#include <dx12pch.h>

#include "Common/DeviceResources.h"


#include "../IGraphicsEngine.h"

#include "Helpers/D3D12Fence/FenceManager.h"
#include "Helpers/DXGISwapChain/SwapChainManager.h"
#include "Helpers/DX12DepthStencilBuffer/DepthStencilManager.h"

#include "Helpers/Camera/Camera.h"
#include "Helpers/InputAssembler/RenderObjects.h"
#include "Helpers/RootSignature/RootSignature.h"
#include "Helpers/Shaders/HLSLShaders.h"
#include "Helpers/Frames/FrameCycle.h"
#include "Helpers/InputAssembler/RenderItem.h"
#include "Helpers/Descriptors/GeneralResource.h"


namespace // engine constants
{
	static constexpr auto BACK_BUFFER_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	static constexpr auto BACK_BUFFER_COUNT = 3; // == # frame resources / canvas to draw

	static constexpr auto DEPTH_BUFFER_FORMAT = DXGI_FORMAT_D32_FLOAT;
	static constexpr auto MSAA_DEPTH_BUFFER_FORMAT = DXGI_FORMAT_UNKNOWN; /* If we were only doing MSAA rendering, we could skip the non-MSAA depth/stencil buffer with DXGI_FORMAT_UNKNOWN */

	static constexpr u32 TARGET_SAMPLE_COUNT = 4;
}


class DX12Engine : public IGraphicsEngine, public DX::IDeviceNotify
{
	DX::DeviceResources _resources;

	// DirectX Tool Kit 12
	std::unique_ptr<DescriptorHeap>		_resourceDescriptors;
	std::unique_ptr<GraphicsMemory>		_graphicsMemory;
	
	ID3D12GraphicsCommandList* m_CommandList;

	std::unique_ptr<Camera> _Camera;
	std::unique_ptr<SwapChainManager> _SwapChain;
	std::unique_ptr<DepthStencilManager> _DepthStencil;
	std::unique_ptr<RootSignature> _RootSignature;
	std::unique_ptr<HLSLShaders> _Shaders;
	std::unique_ptr<FrameCycle> _FrameCycle;

	DXGI_SAMPLE_DESC _MSAA = { 4, 0 }; // This is the default - TODO: make it work with count > 1
	D3D12_VIEWPORT _ScreenViewport;
	D3D12_RECT _ScissorRect;

	// by the book
	std::unique_ptr<RenderObjects> _Objects;
	ComPtr<ID3D12PipelineState> m_PSO = nullptr;

	enum Descriptors
	{
		BrickTexture,
		Count // the last enum obj is the count of items
	};

public:
	DX12Engine();
	~DX12Engine();

	void Initialize(HWND window, u16 width, u16 height) override;

	// IDeviceNotify
	void OnDeviceLost() override;
	void OnDeviceRestored() override;

private:
	void BuildPipelineStateObject();
	void BuildGeometry();

	void ShowFrameStats(milliseconds& dt);

protected:
	void ResetCommandList();
	void CloseCommandList();
	void ExecuteCommandLists();
	void SignalFrameAndAdvance();

public:
	inline bool IsReady() override { return _resources.GetD3DDevice() && _SwapChain->IsReady() && _FrameCycle; };
	void SetCameraPosition(CameraPosition3D pos) override;
	void OnUpdate(milliseconds dt) override;
	void OnDraw() override;
	void OnResize(u16 width = NULL, u16 height = NULL) override;

	// no copy, no move
	DX12Engine(DX12Engine& rhs) = delete;
	DX12Engine(const DX12Engine& rhs) = delete;
	DX12Engine& operator=(DX12Engine& rhs) = delete;
	DX12Engine& operator=(const DX12Engine& rhs) = delete;
};