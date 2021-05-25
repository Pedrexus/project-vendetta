#pragma once

#include <pch.h>
#include <types.h>

#include <dx12pch.h>

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


class DX12Engine : public IGraphicsEngine
{
	ComPtr<IDXGIFactory> _Factory;
	ComPtr<ID3D12Device> _Device;

	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;

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

	Mesh _Sphere;

public:
	DX12Engine() = default;
	DX12Engine(const DX12Engine & rhs) = delete;
	DX12Engine& operator=(const DX12Engine & rhs) = delete;
	~DX12Engine();

public:
	void Initialize() override;

private:
	void BuildPipelineStateObject();
	void BuildGeometry();

	void ShowFrameStats(milliseconds& dt);

protected:
	void FlushCommandQueue();
	void ResetCommandList();
	void CloseCommandList();
	void ExecuteCommandLists();
	void SignalFrameAndAdvance();
	
public:
	inline bool IsReady() override { return _Device && _SwapChain->IsReady() && _FrameCycle; };
	void SetCameraPosition(CameraPosition3D pos) override;
	void OnUpdate(milliseconds dt) override;
	void OnDraw() override;
	void OnResize(u32 width = NULL, u32 height = NULL) override;
};