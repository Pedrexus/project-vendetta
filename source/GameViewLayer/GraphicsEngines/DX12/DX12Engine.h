#pragma once

#include <pch.h>
#include <types.h>
#include "dx12pch.h"

#include "../IGraphicsEngine.h"

#include "Helpers/D3D12Fence/FenceManager.h"
#include "Helpers/DXGISwapChain/SwapChainManager.h"
#include "Helpers/DX12DepthStencilBuffer/DepthStencilManager.h"

#include "Helpers/DX12Descriptor.h"
#include "Helpers/DX12Buffers.h"
#include "Helpers/DX12InputAssembler.h"
#include "Helpers/Camera/Camera.h"

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};


class DX12Engine : public IGraphicsEngine
{
	ComPtr<IDXGIFactory> m_dxgiFactory;
	ComPtr<ID3D12Device> m_d3dDevice;

	std::unique_ptr<FenceManager> m_fence;

	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_CmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	std::unique_ptr<SwapChainManager> m_SwapChain;
	std::unique_ptr<DepthStencilManager> m_DepthStencil;
	std::unique_ptr<Camera> m_Camera;

	DXGI_SAMPLE_DESC m_msaa;
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	// by the book
	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

	std::unique_ptr<MeshGeometry> m_BoxGeo = nullptr;

	ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	ComPtr<ID3DBlob> m_psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;

protected:
	u32 m_MSAA_sampleCount = 1; // TODO: make it work
	u32 m_MSAA_numQualityLevels = 0;

public:
	DX12Engine() = default;
	DX12Engine(const DX12Engine & rhs) = delete;
	DX12Engine& operator=(const DX12Engine & rhs) = delete;
	~DX12Engine();

public:
	void Initialize() override;
private:
	void CheckMSAASupport();
	void CreateCommandObjects();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineStateObject();
	void BuildBoxGeometry();

public:
	void ExecuteCommandLists();
	void FlushCommandQueue();
	
public:
	inline bool IsReady() override { return m_d3dDevice && m_SwapChain->IsReady() && m_CmdListAlloc; };

	inline void SetCameraPosition(CameraPosition3D pos) override
	{
		m_Camera->UpdateCameraView(pos);
	};

	void OnUpdate(milliseconds dt) override;
	void OnDraw() override;
	void OnResize(u32 width = NULL, u32 height = NULL) override;
};