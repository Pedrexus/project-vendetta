#pragma once

#include <pch.h>
#include <types.h>
#include "dx12pch.h"

#include "../IGraphicsEngine.h"

#include "Helpers/DX12Descriptor.h"
#include "Helpers/DX12Buffers.h"
#include "Helpers/DX12InputAssembler.h"

inline XMFLOAT4X4 Identity4x4()
{
	static XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = Identity4x4();
};


class DX12Engine : public IGraphicsEngine
{
	ComPtr<IDXGIFactory> m_dxgiFactory;
	ComPtr<ID3D12Device> m_d3dDevice;

	ComPtr<ID3D12Fence> m_fence;
	u64 m_CurrentFence = 0;

	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_CmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	ComPtr<IDXGISwapChain> m_swapChain;

	i32 m_CurrBackBuffer = 0;
	static constexpr auto SwapChainBufferCount = 2;
	ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];

	ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	Descriptor::Heap m_RtvHeap;
	Descriptor::Heap m_DsvHeap;
	Descriptor::Heap m_CbvHeap;

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS m_msaa;

	XMVECTOR m_cameraPosition;

	// by the book
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	std::unique_ptr<UploadBuffer<ObjectConstants>> m_ObjectCB = nullptr;
	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

	std::unique_ptr<MeshGeometry> m_BoxGeo = nullptr;

	ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	ComPtr<ID3DBlob> m_psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;

	XMFLOAT4X4 m_World = Identity4x4();
	XMFLOAT4X4 m_View = Identity4x4();
	XMFLOAT4X4 m_Proj = Identity4x4();

protected:
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	u32 m_MSAA_sampleCount = 1;
	u32 m_MSAA_numQualityLevels = 0;

public:
	DX12Engine() = default;
	DX12Engine(const DX12Engine & rhs) = delete;
	DX12Engine& operator=(const DX12Engine & rhs) = delete;
	~DX12Engine();

public:
	void Initialize() override;
private:
	void CreateDXGIFactoryWithDebugLayer();
	void CreateHardwareDeviceFromAdaptersOrderedByPerformance();
	void CreateFence();
	void CheckMSAASupport();
	void CreateCommandObjects();
	void CreateDoubleBufferingSwapChain();
	void CreateDescriptorHeaps();

	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineStateObject();
	void BuildBoxGeometry();

public:
	void ExecuteCommandLists();
	void FlushCommandQueue();
	inline ID3D12Resource* GetCurrentBackBuffer() { return m_SwapChainBuffer[m_CurrBackBuffer].Get(); }

public:
	inline bool IsReady() override { return m_d3dDevice && m_swapChain && m_CmdListAlloc; };

	inline void SetCameraPosition(CameraPosition3D pos) override
	{
		m_cameraPosition = XMVectorSet(pos[0], pos[1], pos[2], 1.0f);
	};

	void OnUpdate(milliseconds dt) override;
	void OnDraw() override;
	void OnResize(u32 width = NULL, u32 heigth = NULL) override;
};