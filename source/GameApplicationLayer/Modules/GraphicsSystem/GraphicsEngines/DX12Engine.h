#pragma once

#include "dx12pch.h"
#include "WindowManager.h"


using Microsoft::WRL::ComPtr;

#define ThrowIfFailed(hr) if(FAILED(hr)) { LOG_FATAL("DirectX has failed with HRESULT " + Convert::int2hex(hr)); }

/* Responsible for
	- Device communication
	- Resource Initialization
	- Window Management
	- CPU - GPU Synchronization (for optimal performance, keep both busy for as long as possible and minimize synchronizations)

TODO: Once the reader is finished with this book, we recommend they study the Multithreading12 SDK sample to see how command lists can be generated in parallel.
*/
class DX12Engine : public WindowManager
{
	Microsoft::WRL::ComPtr<IDXGIFactory7> m_dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device8> m_d3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	u64 m_CurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	i32 m_CurrBackBuffer = 0;
	static const i32 SwapChainBufferCount = 2;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

	u32 m_RtvDescriptorSize = 0;
	u32 m_DsvDescriptorSize = 0;
	u32 m_CbvSrvUavDescriptorSize = 0;

	// Set true to use 4X MSAA (4.1.8).
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS m_MSAA = {};
	D3D12_VIEWPORT m_ScreenViewport = {};
	D3D12_RECT m_ScissorRect = {};

protected:
	// Derived class should set these in derived constructor to customize starting values.
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// initialization
	bool InitializeDirect3D(); // 3D Graphics API
	void CreateHardwareDevice();

	// descriptors accessors
	inline ID3D12Resource* GetCurrentBackBuffer() const
	{
		return m_SwapChainBuffer[m_CurrBackBuffer].Get();
	}
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle() const
	{
		// We have 2 RTV buffers. Therefore, this helps specifiyng when we are using one or the other.
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrBackBuffer, m_RtvDescriptorSize);
	}
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilHandle() const
	{
		return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
		// NEVER DO THIS: it changes the heap start to something that isn't valid
		// return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DsvHeap->GetCPUDescriptorHandleForHeapStart(), m_DsvDescriptorSize);
	}

public:
	inline DX12Engine(HINSTANCE hInstance) : WindowManager(hInstance), m_SwapChain(nullptr) {};
	virtual inline ~DX12Engine() { OnDestroy(); };

	inline bool IsReady() { return m_d3dDevice; }

	// DirectX Graphics Infrastructure (DXGI): tasks common to multiple graphics APIs
	std::vector<IDXGIAdapter4*> GetDisplayAdapters(); // Display adapters implement graphical functionality (hardware or software)

	inline void OnInit() { InitializeMainWindow() && InitializeDirect3D(); OnResize(); }
	inline void OnUpdate() {};
	inline void OnRender() {};

	void Draw();

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