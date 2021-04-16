#include "DX12Engine.h"

#include "Helpers/DX12Debug.h"
#include "Helpers/DX12DXGI.h"
#include "Helpers/DX12Device.h"
#include "Helpers/DX12MSAA.h"
#include "Helpers/DX12Command.h"
#include "Helpers/DX12Shaders.h"
#include "Helpers/DX12Window.h"
#include "Helpers/Buffers/DefaultBuffer.h"
#include "Helpers/InputAssembler/Vertex.h"

#include <GameLogicLayer/Game.h>


DX12Engine::~DX12Engine()
{
	if (m_d3dDevice)
		FlushCommandQueue();
}

void DX12Engine::Initialize()
{
#ifdef _DEBUG
	EnableDebugLayer();
#endif

	m_dxgiFactory = DXGI::Factory::CreateWithDebugLayer();
	m_d3dDevice = CreateHardwareDeviceWithHighestPerformanceAdapterAvailable(m_dxgiFactory.Get());
	
	CheckMSAASupport();
	CreateCommandObjects();
	
	m_fence = std::make_unique<FenceManager>(m_d3dDevice.Get());
	m_SwapChain = std::make_unique<SwapChainManager>(m_dxgiFactory.Get(), m_d3dDevice.Get(), m_CommandQueue.Get(), m_msaa);
	m_DepthStencil = std::make_unique<DepthStencilManager>(m_d3dDevice.Get());
	m_Camera = std::make_unique<Camera>(m_d3dDevice.Get());
	_RootSignature = std::make_unique<RootSignature>(m_d3dDevice.Get(), 1);
	_Shaders = std::make_unique<HLSLShaders>(L"Shaders\\color.hlsl", nullptr);

#ifdef _DEBUG
	Display::LogInformation(m_dxgiFactory.Get(), m_SwapChain->BackBufferFormat);
#endif

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(m_CommandList->Reset(m_CmdListAlloc.Get(), nullptr));

	BuildPipelineStateObject();
	BuildBoxGeometry();

	// Execute the initialization commands.
	ThrowIfFailed(m_CommandList->Close());
	ExecuteCommandLists();
}

void DX12Engine::ExecuteCommandLists()
{
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void DX12Engine::FlushCommandQueue()
{
	// Add an instruction to the command queue to set a new fence point.  
	// Set a fence from the GPU side.
	// The new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(m_CommandQueue->Signal(m_fence->Get(), m_fence->Advance()));
	if (!m_fence->IsSynchronized())
		m_fence->WaitForGPU();
}

void DX12Engine::CheckMSAASupport()
{
	auto msaa = MSAA::Specify(m_SwapChain->BackBufferFormat, m_MSAA_sampleCount, m_MSAA_numQualityLevels);
	ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaa, sizeof(msaa)));

	if (msaa.NumQualityLevels < 0)
		LOG_ERROR("Unexpected MSAA quality level.");
	LOG("Graphics", fmt::format("Using MSAA with {} samples and {} quality levels", msaa.SampleCount, msaa.NumQualityLevels));

	m_msaa = { msaa.SampleCount, msaa.NumQualityLevels - 1 };
}

void DX12Engine::CreateCommandObjects()
{
	Command::CreateQueue(m_d3dDevice.Get(), m_CommandQueue.GetAddressOf());
	Command::CreateAllocator(m_d3dDevice.Get(), m_CmdListAlloc.GetAddressOf());
	Command::CreateList(m_d3dDevice.Get(), m_CmdListAlloc.Get(), m_CommandList.GetAddressOf());

	// The command list must be closed before passing it off to the GPU.
	ThrowIfFailed(m_CommandList->Close());
}

void DX12Engine::BuildBoxGeometry()
{
	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT) vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT) indices.size() * sizeof(std::uint16_t);

	m_BoxGeo = std::make_unique<MeshGeometry>();
	m_BoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_BoxGeo->VertexBufferCPU));
	CopyMemory(m_BoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_BoxGeo->IndexBufferCPU));
	CopyMemory(m_BoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_BoxGeo->VertexBufferGPU = std::make_shared<DefaultBuffer>(m_d3dDevice.Get(),
		m_CommandList.Get(), vertices.data(), vbByteSize);

	m_BoxGeo->IndexBufferGPU = std::make_shared<DefaultBuffer>(m_d3dDevice.Get(),
		m_CommandList.Get(), indices.data(), ibByteSize);

	m_BoxGeo->VertexByteStride = sizeof(Vertex);
	m_BoxGeo->VertexBufferByteSize = vbByteSize;
	m_BoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_BoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT) indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_BoxGeo->DrawArgs["box"] = submesh;
}

void DX12Engine::BuildPipelineStateObject()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = SpecifyInputLayout();
	psoDesc.pRootSignature = _RootSignature->Get();
	psoDesc.VS = _Shaders->GetVSByteCode();
	psoDesc.PS = _Shaders->GetPSByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT32_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_SwapChain->BackBufferFormat;
	psoDesc.SampleDesc = m_msaa;
	psoDesc.DSVFormat = m_DepthStencil->depthStencilFormat;
	ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));
}

void DX12Engine::OnUpdate(milliseconds dt)
{
	
	// frame stats
	static auto presentCount = 0;
	static f64 elapsedTimeSinceLastWrite = 0;

	auto stats = m_SwapChain->GetFrameStatistics();

	elapsedTimeSinceLastWrite += dt / 1000.0f;
	if (elapsedTimeSinceLastWrite >= .5f)
	{
		auto fps = (stats.PresentCount - presentCount) / elapsedTimeSinceLastWrite;
		auto mspf = 1.0f / fps;
		
		// TODO: - camera: ({:.2f}, {:.2f}, {:.2f})
		auto windowText = fmt::format(L"fps: {:.0f} mspf: {:.6f}", fps, mspf);
		SetWindowText(Game::Get()->GetWindow()->GetMainWnd(), windowText.c_str());

		elapsedTimeSinceLastWrite = 0;
		presentCount = stats.PresentCount;
	}
	
}

void DX12Engine::OnDraw()
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(m_CmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_CommandList->Reset(m_CmdListAlloc.Get(), m_PSO.Get()));

	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	const auto currentBackBuffer = m_SwapChain->GetCurrentBackBuffer();

	// Indicate a state transition on the resource usage.
	auto renderTransition = m_SwapChain->GetPresentTransition();
	m_CommandList->ResourceBarrier(1, &renderTransition);

	const auto cbbCPUHandle = m_SwapChain->GetCurrentBackBufferCPUHandle();
	const auto dsCPUHandle = m_DepthStencil->GetCPUHandle();

	// Clear the back buffer and depth buffer.
	m_CommandList->ClearRenderTargetView(cbbCPUHandle, Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	m_CommandList->OMSetRenderTargets(1, &cbbCPUHandle, true, &dsCPUHandle);

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_Camera->GetBufferHeap() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_CommandList->SetGraphicsRootSignature(_RootSignature->Get());

	auto boxVertexView = m_BoxGeo->VertexBufferView();
	m_CommandList->IASetVertexBuffers(0, 1, &boxVertexView);

	auto boxIndexView = m_BoxGeo->IndexBufferView();
	m_CommandList->IASetIndexBuffer(&boxIndexView);
	m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_CommandList->SetGraphicsRootDescriptorTable(0, m_Camera->GetBufferGPUHandle());

	m_CommandList->DrawIndexedInstanced(
		m_BoxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);

	// Indicate a state transition on the resource usage.
	auto presentTransition = m_SwapChain->GetPresentTransition();
	m_CommandList->ResourceBarrier(1, &presentTransition);

	// Done recording commands.
	ThrowIfFailed(m_CommandList->Close());

	// Add the command list to the queue for execution.
	ExecuteCommandLists();

	m_SwapChain->Present();
	
	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}

void DX12Engine::OnResize(u32 width, u32 height)
{
	if (!IsReady())
		LOG_FATAL("Called OnResize before graphics engine was ready");
	ASSERT(width != NULL && height != NULL);

	// Flush before changing any resources.
	FlushCommandQueue();

	ThrowIfFailed(m_CommandList->Reset(m_CmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	m_SwapChain->Resize(m_d3dDevice.Get(), width, height);
	m_DepthStencil->Resize(m_d3dDevice.Get(), width, height, m_msaa);

	auto dsWriteTransition = m_DepthStencil->GetWriteTransition();
	m_CommandList->ResourceBarrier(1, &dsWriteTransition);

	// Execute the resize commands.
	ThrowIfFailed(m_CommandList->Close());
	ExecuteCommandLists();

	// Wait until resize is complete.
	FlushCommandQueue();

	m_ScreenViewport = CreateViewport(width, height);
	m_ScissorRect = CreateScissorRectangle(width, height);
	m_Camera->Resize(width, height);
}
