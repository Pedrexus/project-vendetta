#include "DX12Engine.h"
#include "DX12Helpers.h"

#include <const.h>
#include <helpers.h>


/*
  Breakdown
	0. Enable D3D12 Debug Layer
	1. Create the ID3D12Device using the D3D12CreateDevice function.
	2. Create an ID3D12Fence object
	3. Check 4X MSAA quality level support.
	4. Create the command queue, command list allocator, and main command list.
	5. Describe and create the swap chain.
	6. Query descriptor sizes and create the descriptor heaps the application requires.
*/
bool DX12Engine::InitializeDirect3D()
{
#ifdef _DEBUG
	// Enable the D3D12 debug layer.
	// Direct3D will enable extra debugging and send debug messages to the VC++ output window
	{
		ComPtr<ID3D12Debug3> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	// CreateDXGIFactory Flags
#ifdef _DEBUG
	UINT CREATE_FACTORY_FLAGS = DXGI_CREATE_FACTORY_DEBUG;
#else
	UINT CREATE_FACTORY_FLAGS = 0;
#endif
	ThrowIfFailed(CreateDXGIFactory2(CREATE_FACTORY_FLAGS, IID_PPV_ARGS(&m_dxgiFactory)));

#ifdef _DEBUG
	// LogDisplayInformation();
#endif

	CreateHardwareDevice();

	/*
		2. Create an ID3D12Fence object

		a fence is identified by an integer.
		At each fence point, we increment it.
	*/
	{
		ASSERT(m_CurrentFence == 0);
		ThrowIfFailed(m_d3dDevice->CreateFence(
			m_CurrentFence,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_Fence))
		);
	}

	/*
		3. Check 4X MSAA quality level support.

		MSAA = Multisample anti-aliasing

		this makes the RSV and DSV buffers 4x larger.

		Check 4X MSAA quality support for our back buffer format.
		All Direct3D 11 capable devices support 4X MSAA for all render
		target formats, so we only need to check quality support.
	*/
	{
		m_MSAA = MSAA::Create(m_BackBufferFormat, 4, 0);
		ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &m_MSAA, sizeof(m_MSAA)));

		if (m_MSAA.NumQualityLevels < 0)
			LOG_ERROR("Unexpected MSAA quality level.");
		LOG("Graphics", fmt::format("Using MSAA with {} samples and {} quality levels", m_MSAA.SampleCount, m_MSAA.NumQualityLevels));
	}

	/*
		4. Create Command Objects

		the GPU recieves commands through a command queue.
		for high performance applications, the goal is to
		keep both CPU and GPU busy (reduce idle time)

		-> keep the queue filled but not overloaded
	*/
	{
		// GPU owns a command queue
		// Commands sit in the queue until the GPU is ready to process them
		auto queueDesc = Command::Queue();
		ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

		// The command list saves each command on a Command Allocator
		ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, // DIRECT or BUNDLE (bundle is an optimized version of the command list)
			IID_PPV_ARGS(m_DirectCmdListAlloc.GetAddressOf())
		));

		// CPU submits commands to the queue through the Direct3D API using command lists
		ThrowIfFailed(m_d3dDevice->CreateCommandList(
			0, // node mask: 0 for a single GPU system
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_DirectCmdListAlloc.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject should be null
			IID_PPV_ARGS(m_CommandList.GetAddressOf())
		));

		// The command list must be closed before passing it off to the GPU.

		// Start off in a closed state.  
		// This is because the first time we refer to 
		// the command list we will Reset it, and it 
		// needs to be closed before calling Reset.
		ThrowIfFailed(m_CommandList->Close());
	}

	/*
		5. Creates the Double Buffering Swap Chain

		front buffer: stores the image data currently being displayed on the monitor
		back buffer: off-screen texture where the frame is being drawn
		presenting: Swapping the roles of the back and front buffers after draw is done.

		The front and back buffer form a swap chain.
	*/
	{
		// Release the previous swapchain we will be recreating.
		m_SwapChain.Reset();

		auto swapChainDesc = SwapChain::Create(
			m_ClientWidth,
			m_ClientHeight,
			m_BackBufferFormat,
			1, // DX12 SwapChain SampleDesc.Count > 1 fails
			0, // m_MSAA.NumQualityLevels - 1,
			SwapChainBufferCount
		);

		auto fsSwapChainDesc = SwapChain::Fullscreen(
			SCREEN_REFRESH_RATE // TODO: get from output 
		);

		// Note: Swap chain uses queue to perform flush.
		ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
			m_CommandQueue.Get(),
			m_hMainWnd,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			(IDXGISwapChain1**) m_SwapChain.GetAddressOf()
		));
	}

	/*
		6. GPU Descriptors

		a GPU resource is referenced through a descriptor object,
		which can be thought of as lightweight structure that describes the resource to the GPU.

		GPU resources are essentially generic chunks of memory. The descriptor reserves it for a purpose.

		In addition to identifying the resource data, descriptors describe the resource to the GPU:
		they tell Direct3D how the resource will be used (i.e., what stage of the pipeline you will bind it to)

		Descriptor types:
		1. CBV/SRV/UAV descriptors describe constant buffers, shader resources and unordered access view resources.
		2. Sampler descriptors describe sampler resources (used in texturing).
		3. RTV descriptors describe render target resources.
		4. DSV descriptors describe depth/stencil resources.

		-> descriptor heap = array of descriptors (block of GPU memory)
		-> descriptors should be created at initialization time.
		-> descriptor == view
	*/
	{
		// Gets the size of the handle increment for the given type of descriptor heap.
		// This value is typically used to increment a handle into a descriptor array by the correct amount.
		m_RtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_CbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		auto rtvHeapDesc = Descriptor::RenderTarget::Heap(SwapChainBufferCount);
		ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));

		auto dsvHeapDesc = Descriptor::DepthStencil::Heap();
		ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
	}

	return true;
}

// Try to create hardware device.
void DX12Engine::CreateHardwareDevice()
{
	// A device represents the display adapter (graphics card). Direct3D 12 devices are singletons per adapter.
	HRESULT createDeviceResult = E_FAIL;
	for (auto& adapter : GetDisplayAdapters())
	{
		createDeviceResult = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
		DXGI_ADAPTER_DESC3 adapterDesc; adapter->GetDesc3(&adapterDesc);
		if (SUCCEEDED(createDeviceResult))
		{
			LOG("Graphics", "D3D12 Device created with " + Convert::ws2s(adapterDesc.Description));
			break;
		}
		else
			LOG_WARNING("Failure at D3D12CreateDevice with " + Convert::ws2s(adapterDesc.Description));
	}

	// Fallback to WARP device.
	if (FAILED(createDeviceResult))
	{
		LOG_WARNING("Failure at D3D12CreateDevice with Highest Performance Adapter. Falling back to WARP device.");

		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice)));
	}
}

std::vector<IDXGIAdapter4*> DX12Engine::GetDisplayAdapters()
{
	UINT i = 0;
	IDXGIAdapter4* adapter = nullptr;
	std::vector<IDXGIAdapter4*> adapterList;
	while (m_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND)
	{
		adapterList.push_back(adapter);
		++i;
	}

	return adapterList;
}

void DX12Engine::Draw()
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(m_DirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	const auto currentBackBuffer = GetCurrentBackBuffer();

	// Indicate a state transition on the resource usage.
	auto renderTransition = CD3DX12_RESOURCE_BARRIER::Transition(
		currentBackBuffer,
		D3D12_RESOURCE_STATE_PRESENT, // before
		D3D12_RESOURCE_STATE_RENDER_TARGET // after
	);
	m_CommandList->ResourceBarrier(1, &renderTransition);

	// Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	const auto currentBackBufferHandle = GetCurrentBackBufferHandle();
	const auto depthStencilHandle = GetDepthStencilHandle();

	// Clear the back buffer and depth buffer.
	m_CommandList->ClearRenderTargetView(currentBackBufferHandle, DirectX::Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(depthStencilHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	m_CommandList->OMSetRenderTargets(1, &currentBackBufferHandle, true, &depthStencilHandle);

	// Indicate a state transition on the resource usage.
	auto presentTransition = CD3DX12_RESOURCE_BARRIER::Transition(
		currentBackBuffer,
		D3D12_RESOURCE_STATE_RENDER_TARGET, // before
		D3D12_RESOURCE_STATE_PRESENT // after
	);
	m_CommandList->ResourceBarrier(1, &presentTransition);

	// Done recording commands.
	ThrowIfFailed(m_CommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(m_SwapChain->Present(0, 0)); // Presents a rendered image to the user (not really part of the draw process...)
	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}

void DX12Engine::OnDestroy()
{
	if (m_d3dDevice)
		FlushCommandQueue();
}

void DX12Engine::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	m_CurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (m_Fence->GetCompletedValue() < m_CurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, L"Flush Command Queue Event", false, EVENT_ALL_ACCESS);

		if (GetLastError() == ERROR_ALREADY_EXISTS)
			LOG_ERROR("Flush Command Queue Event Already exists. GPU may be left on a broken state.");

		if (eventHandle)
		{
			// Fire event when GPU hits current fence.  
			ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
		else
			LOG_ERROR("Event Handle is null");
	}
}

/*
	7. Resize the back buffer and create a render target view to the back buffer.
	8. Create the depth / stencil buffer and its associated depth / stencil view.
	9. Set the viewport and scissor rectangles.
*/
void DX12Engine::OnResize(u32 width, u32 height)
{
	ASSERT(m_d3dDevice);
	ASSERT(m_SwapChain);
	ASSERT(m_DirectCmdListAlloc);

	if (width > 0)
		m_ClientWidth = width;
	if (height > 0)
		m_ClientHeight = height;

	// Flush before changing any resources.
	FlushCommandQueue();

	// resets the command list to its initial state
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	/*
		7. Resize the back buffer and create a
		render target view to the back buffer.

		Resize the swap chain.
	*/
	{
		// Release the previous resources we will be recreating.
		for (int i = 0; i < SwapChainBufferCount; ++i)
			m_SwapChainBuffer[i].Reset();

		ThrowIfFailed(m_SwapChain->ResizeBuffers(
			SwapChainBufferCount,
			m_ClientWidth,
			m_ClientHeight,
			m_BackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		));

		m_CurrBackBuffer = 0;

		/*
			A render target is any intermediate memory buffer
			the GPU uses to render a 3D scene.

			A view is a synonym for descriptor in Direct3D 12.
			A descriptor is a lightweight structure that describes the resource to the GPU.
		*/
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (u8 i = 0; i < SwapChainBufferCount; i++)
		{
			ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
			m_d3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, m_RtvDescriptorSize); // Sets the offset for 1 descriptor using the RTV increment size. Works like a file.seek() 
		}
	}

	/*
		8. Create the depth / stencil buffer and
		its associated depth / stencil view.
	*/
	{
		// Release the previous resources we will be recreating.
		m_DepthStencilBuffer.Reset();

		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		// typeless format allows us to create two views
		// Here we define the properties of the depth resource descriptor
		auto format = m_DepthStencilFormat;
		auto dsBufferDesc = Descriptor::DepthStencil::Buffer(m_ClientWidth, m_ClientHeight, format); // TODO: DXGI_FORMAT_R24G8_TYPELESS

		// describes an optimized value for clearing resources
		auto dsClear = Descriptor::DepthStencil::ClearValue(format); // TODO: DXGI_FORMAT_R24G8_TYPELESS

		// Default type = resource will only be accessed by the GPU (default heap for optimal performance)
		auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&dsBufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&dsClear,
			IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())
		));

		/*
			The depth buffer is just a 2D texture that stores
			the depth information of the nearest visible objects
			(and stencil information if using stenciling).
		*/
		auto dsvDesc = Descriptor::DepthStencil::View(m_DepthStencilFormat); // Create descriptor to mip level 0 of entire resource using the format of the resource.
		m_d3dDevice->CreateDepthStencilView(
			m_DepthStencilBuffer.Get(),
			&dsvDesc,
			GetDepthStencilHandle()
		);

		// Transition the resource from its initial state 
		// to be used as a depth buffer.
		auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
			m_DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,  // initial state
			D3D12_RESOURCE_STATE_DEPTH_WRITE // after
		);
		m_CommandList->ResourceBarrier(1, &transition);
	}

	// Done recording commands.
	ThrowIfFailed(m_CommandList->Close());

	// Execute the resize commands.
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();

	// 9. Set the viewport and scissor rectangles.
	{
		/*
			A viewport is a subrectangle we can draw into
			the back buffer (like a subscreen).

			It can be used to implement split screens for
			two-player game modes.

			Update the viewport transform to cover the client area.
		*/
		m_ScreenViewport = CreateViewport(m_ClientWidth, m_ClientHeight);

		/*
			A scissor rectangle is a simple rectangle that prevents
			the back buffer from rendering a certain area.

			This is used for optimizations:
			if we have a rect UI element covering part of the screen,
			we can avoid drawing on that region.
		*/
		m_ScissorRect = CreateScissorRectangle(0, 0, m_ClientWidth, m_ClientHeight);
	}
}

void DX12Engine::LogDisplayInformation()
{
	std::string text = "\n";
	for (auto& adapter : GetDisplayAdapters())
	{
		DXGI_ADAPTER_DESC3 adapterDesc; adapter->GetDesc3(&adapterDesc);
		text += fmt::format("Adapter {}: {}\n", adapterDesc.DeviceId, Convert::ws2s(adapterDesc.Description));
		for (auto& output : Display::AdapterOutputs(adapter))
		{
			DXGI_OUTPUT_DESC outputDesc; output->GetDesc(&outputDesc);
			text += fmt::format("Output: {}\n", Convert::ws2s(outputDesc.DeviceName));
			for (auto& mode : Display::OutputModes(static_cast<IDXGIOutput1*>(output), m_BackBufferFormat))
				text += fmt::format("Mode: {}x{}\t{}Hz\n", mode.Width, mode.Height, mode.RefreshRate.Numerator / mode.RefreshRate.Denominator);
		}
	}
	LOG("Graphics", text);
}
