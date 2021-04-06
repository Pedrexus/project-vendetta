#include "DX12Engine.h"
#include "DX12Helpers.h"
#include "Shaders/ShaderCompiler.h"

#include <const.h>




void DX12Engine::InitializeEngineResources()
{
	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	// Build Constant Buffers
	{
		m_constantBuffer = std::make_unique<InputAssembler::UploadBuffer<InputAssembler::Constants>>(m_d3dDevice.Get(), 1, true);

		auto cbvDesc = m_constantBuffer->GetConstantBufferView();
		m_d3dDevice->CreateConstantBufferView(&cbvDesc, GetConstantBufferHandle());
	}
	
	// Build Root Signature
	{
		// Shader programs typically require resources as input (constant buffers,
		// textures, samplers).  The root signature defines the resources the shader
		// programs expect.  If we think of the shader programs as a function, and
		// the input resources as function parameters, then the root signature can be
		// thought of as defining the function signature. 

		// For performance, make the root signature as small as possible, and try to
		// minimize the number of times you change the root signature per rendering frame.

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[1] = {};

		// Create a single descriptor table of CBVs.
		CD3DX12_DESCRIPTOR_RANGE cbvTable = {};
		cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
			1, 
			slotRootParameter, 
			0, 
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		// create a root signature with a single slot which points 
		// to a descriptor range consisting of a single constant buffer
		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(
			&rootSigDesc, 
			D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), 
			errorBlob.GetAddressOf()
		);

		if (errorBlob != nullptr)
			LOG_ERROR((char*)errorBlob->GetBufferPointer());
		ThrowIfFailed(hr);

		ThrowIfFailed(m_d3dDevice->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&m_RootSignature))
		);
	}

	// Build Pipeline State Object
	{
		/*
			objects that control the state of the graphics pipeline are specified as an
			aggregate called a pipeline state object
			
			By specifying them as an aggregate, Direct3D can validate that all the state is compatibleand
			the driver can generate all the code up front to program the hardware state
			
			Because PSO validation and creation can be time consuming, 
			PSOs should be generated at initialization time.
			
			when a PSO is bound to the command list, it does not change until you
			overwrite it(or the command list is reset). - Do not change the PSO per draw call!
		*/
		auto vs = Shaders::Compile(L"color.hlsl", nullptr, "VS", "vs_5_0");
		auto ps = Shaders::Compile(L"color.hlsl", nullptr, "PS", "ps_5_0");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesc.InputLayout.pInputElementDescs = InputAssembler::VertexInputLayout;
		psoDesc.InputLayout.NumElements = _countof(InputAssembler::VertexInputLayout);
		psoDesc.pRootSignature = m_RootSignature.Get();
		psoDesc.VS.pShaderBytecode = reinterpret_cast<BYTE*>(vs->GetBufferPointer());
		psoDesc.VS.BytecodeLength = vs->GetBufferSize();
		psoDesc.PS.pShaderBytecode = reinterpret_cast<BYTE*>(ps->GetBufferPointer());
		psoDesc.PS.BytecodeLength = ps->GetBufferSize();
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = m_BackBufferFormat;
		psoDesc.DSVFormat = m_DepthStencilFormat;
		psoDesc.SampleDesc.Count = m_MSAA.SampleCount;
		psoDesc.SampleDesc.Quality = m_MSAA.NumQualityLevels - 1;

		ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	// Build Box Geometry
	{
		auto box = std::make_unique<Box>();
		box->Name = "boxGeo";

		const auto vbByteSize = sizeof(box->vertices);
		const auto ibByteSize = sizeof(box->indices);

		// create vertex blob and copy vertices into it
		ThrowIfFailed(D3DCreateBlob(vbByteSize, &box->VertexBufferCPU));
		CopyMemory(box->VertexBufferCPU->GetBufferPointer(), box->vertices, vbByteSize);

		// create index blob and copy indices into it
		ThrowIfFailed(D3DCreateBlob(ibByteSize, &box->IndexBufferCPU));
		CopyMemory(box->IndexBufferCPU->GetBufferPointer(), box->indices, ibByteSize);

		box->VertexBufferGPU = InputAssembler::DefaultBuffer(
			m_d3dDevice.Get(),
			m_CommandList.Get(), 
			box->vertices,
			vbByteSize, 
			box->VertexBufferUploader
		);

		box->IndexBufferGPU = InputAssembler::DefaultBuffer(
			m_d3dDevice.Get(),
			m_CommandList.Get(),
			box->indices,
			ibByteSize,
			box->IndexBufferUploader
		); 
		
		box->VertexByteStride = sizeof(InputAssembler::Vertex);
		box->VertexBufferByteSize = vbByteSize;
		box->IndexFormat = DXGI_FORMAT_R16_UINT;
		box->IndexBufferByteSize = ibByteSize;

		SubmeshGeometry submesh;
		submesh.IndexCount = _countof(box->indices);
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;
		box->DrawArgs["box"] = submesh;

		m_geometry = std::move(box);
	}

	// Execute the initialization commands.
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();
}



void DX12Engine::Draw(XMMATRIX viewMatrix)
{
	// Update the constant buffer with the latest worldViewProj matrix.
	InputAssembler::Constants constants = {};
	XMStoreFloat4x4(&constants.worldViewProj, XMMatrixTranspose(m_World * viewMatrix * m_Proj));
	m_constantBuffer->CopyToBuffer(0, constants);

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(m_DirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));

	// Set the viewport and scissor rect. This needs to be reset whenever the command list is reset.
	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	const auto currentBackBuffer = GetCurrentBackBuffer();

	// Indicate a state transition on the resource usage.
	auto renderTransition = CD3DX12_RESOURCE_BARRIER::Transition(
		currentBackBuffer,
		D3D12_RESOURCE_STATE_PRESENT, // before
		D3D12_RESOURCE_STATE_RENDER_TARGET // after
	);
	m_CommandList->ResourceBarrier(1, &renderTransition);

	const auto currentBackBufferHandle = GetCurrentBackBufferHandle();
	const auto depthStencilHandle = GetDepthStencilHandle();

	// Clear the back buffer and depth buffer.
	m_CommandList->ClearRenderTargetView(currentBackBufferHandle, DirectX::Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(depthStencilHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	m_CommandList->OMSetRenderTargets(1, &currentBackBufferHandle, true, &depthStencilHandle);

	// set and draw the box
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
		 
	auto vbv = m_geometry->VertexBufferView();
	m_CommandList->IASetVertexBuffers(0, 1, &vbv);

	auto ibv = m_geometry->IndexBufferView();
	m_CommandList->IASetIndexBuffer(&ibv);

	m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
	m_CommandList->DrawIndexedInstanced(m_geometry->DrawArgs["box"].IndexCount, 1, 0, 0, 0);
	
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

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	m_Proj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, AspectRatio(), 1.0f, 1000.0f);
}
