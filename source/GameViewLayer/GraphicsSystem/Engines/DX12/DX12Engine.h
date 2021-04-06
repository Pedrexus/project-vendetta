#pragma once

#include "dx12pch.h"
#include "WindowManager.h"
#include "Rendering/InputAssembler.h"
#include "Rendering/Structures/Box.h"

/* Responsible for
	- Device communication
	- Resource Initialization
	- Window Management
	- CPU - GPU Synchronization (for optimal performance, keep both busy for as long as possible and minimize synchronizations)

TODO: Once the reader is finished with this book, we recommend they study the Multithreading12 SDK sample to see how command lists can be generated in parallel.

IA: input assembler
OM: output merger
*/
class DX12Engine : public WindowManager
{
	ComPtr<IDXGIFactory7> m_dxgiFactory;
	ComPtr<IDXGISwapChain4> m_SwapChain;
	ComPtr<ID3D12Device> m_d3dDevice;

	ComPtr<ID3D12RootSignature> m_RootSignature;
	ComPtr<ID3D12PipelineState> m_PipelineState;

	ComPtr<ID3D12Fence> m_Fence;
	u64 m_CurrentFence = 0;

	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	i32 m_CurrBackBuffer = 0;
	static const i32 SwapChainBufferCount = 2;
	ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
	ComPtr<ID3D12DescriptorHeap> m_CbvHeap;

	u32 m_RtvDescriptorSize = 0;
	u32 m_DsvDescriptorSize = 0;
	u32 m_CbvSrvUavDescriptorSize = 0;

	// Set true to use 4X MSAA (4.1.8).
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS m_MSAA = {};
	D3D12_VIEWPORT m_ScreenViewport = {};
	D3D12_RECT m_ScissorRect = {};

	std::unique_ptr<InputAssembler::UploadBuffer<InputAssembler::Constants>> m_constantBuffer;
	std::unique_ptr<Box> m_geometry;

	XMMATRIX m_World = XMMatrixIdentity();
	XMMATRIX m_Proj = XMMatrixIdentity();

protected:
	// Derived class should set these in derived constructor to customize starting values.
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// initialization
	bool InitializeDirect3D(); // 3D Graphics API
	void InitializeEngineResources();
	void CreateHardwareDevice();

	// descriptors accessors
	inline ID3D12Resource* GetCurrentBackBuffer() const
	{
		return m_SwapChainBuffer[m_CurrBackBuffer].Get();
	}

public:
	inline DX12Engine(HINSTANCE hInstance) : WindowManager(hInstance), m_SwapChain(nullptr) {};
	virtual inline ~DX12Engine() { OnDestroy(); };

	inline bool IsReady() { return m_d3dDevice; }

	// DirectX Graphics Infrastructure (DXGI): tasks common to multiple graphics APIs
	std::vector<IDXGIAdapter4*> GetDisplayAdapters(); // Display adapters implement graphical functionality (hardware or software)

	inline void OnInit() { 
		InitializeMainWindow() && InitializeDirect3D(); 
		OnResize();  
		InitializeEngineResources();
	}
	inline void OnUpdate() {};
	inline void OnRender() {};

	void Draw(XMMATRIX viewMatrix);

public:
	void OnDestroy();
protected:
	void FlushCommandQueue();

public:
	void OnResize(u32 width = NULL, u32 heigth = NULL);
protected:
	void CreateRenderTargetView();

protected:
	void LogDisplayInformation();

};