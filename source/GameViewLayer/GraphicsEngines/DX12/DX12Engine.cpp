#include "DX12Engine.h"

#include "Helpers/DX12Debug.h"
#include "Helpers/DX12DXGI.h"
#include "Helpers/DX12Device.h"
#include "Helpers/DX12MSAA.h"
#include "Helpers/DX12Command.h"
#include "Helpers/DX12Window.h"
#include "Helpers/Buffers/DefaultBuffer.h"
#include "Helpers/InputAssembler/Vertex.h"
#include "Helpers/InputAssembler/Objects/Geometry.h"

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

	// TODO: split into Constructor and initialize ?
	m_dxgiFactory = DXGI::Factory::CreateWithDebugLayer();
	m_d3dDevice = CreateHardwareDeviceWithHighestPerformanceAdapterAvailable(m_dxgiFactory.Get());

	m_Camera = std::make_unique<Camera>();

	_RootSignature = std::make_unique<RootSignature>(m_d3dDevice.Get(), 2);
	_Shaders = std::make_unique<HLSLShaders>(L"Shaders\\color.hlsl", nullptr);
	_FrameCycle = std::make_unique<FrameCycle>(m_d3dDevice.Get(), 30);

	_MSAA = MSAA::Check(m_d3dDevice.Get(), m_SwapChain->BackBufferFormat, _MSAA_sampleCount, _MSAA_numQualityLevels);
	LOG_INFO(fmt::format("Using MSAA with {} samples and {} quality levels", _MSAA.Count, _MSAA.Quality));

	CreateCommandObjects();

	m_SwapChain = std::make_unique<SwapChainManager>(m_dxgiFactory.Get(), m_d3dDevice.Get(), m_CommandQueue.Get(), _MSAA);
	m_DepthStencil = std::make_unique<DepthStencilManager>(m_d3dDevice.Get());

#ifdef _DEBUG
	Display::LogInformation(m_dxgiFactory.Get(), m_SwapChain->BackBufferFormat);
#endif

	// Reset the command list to prep for initialization commands.
	ResetCommandList();

	BuildPipelineStateObject();
	BuildGeometry();

	CloseCommandList();
	ExecuteCommandLists();
}

void DX12Engine::ResetCommandList()
{
	auto cmdListAlloc = _FrameCycle->GetCurrentFrameAllocatorWhenAvailable();

	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(m_CommandList->Reset(cmdListAlloc, m_PSO.Get()));
}

void DX12Engine::ExecuteCommandLists()
{
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void DX12Engine::FlushCommandQueue()
{
	_FrameCycle->Flush(m_CommandQueue.Get());
}

void DX12Engine::CreateCommandObjects()
{
	Command::CreateQueue(m_d3dDevice.Get(), m_CommandQueue.GetAddressOf());
	Command::CreateList(m_d3dDevice.Get(), _FrameCycle->GetCurrentFrameAllocatorWhenAvailable(), m_CommandList.GetAddressOf());
	m_CommandList->SetName(L"Main");

	// The command list must be closed before passing it off to the GPU.
	CloseCommandList();
}

void DX12Engine::BuildGeometry()
{
	auto box = Geometry::CreateBox(0, 0, 0);
	auto grid = Geometry::CreateGrid(10, 10, 40, 40);
	auto sphere = Geometry::CreateGeosphere(2.0f, 3);
	auto cylinder = Geometry::CreateCylinder(.8f, .4f, 1.0f, 20, 20);

	std::map<std::string, Mesh*> meshes = {
		{ "box", &box },
		{ "grid", &grid },
		{ "sphere", &sphere },
		{ "cylinder", &cylinder },
	};

	std::vector<std::pair<std::string, XMMATRIX>> items = {
		{ "box", XMMatrixTranslation(.0f, .5f, .0f) },
		{ "grid", XMMatrixIdentity() },

		{ "sphere", XMMatrixTranslation(-5.0f, 3.5f, -5.0f) },
		{ "cylinder", XMMatrixTranslation(-5.0f, 1.5f, -5.0f) },

		{ "sphere", XMMatrixTranslation(+5.0f, 3.5f, -5.0f) },
		{ "cylinder", XMMatrixTranslation(+5.0f, 1.5f, -5.0f) },
	};
	
	_Objects = std::make_unique<RenderObjects>(meshes, items, m_d3dDevice.Get(), m_CommandList.Get());
}

void DX12Engine::ShowFrameStats(milliseconds& dt)
{
	static auto presentCount = 0;
	static f64 elapsedTimeSinceLastWrite = 0;

	auto stats = m_SwapChain->GetFrameStatistics();

	elapsedTimeSinceLastWrite += dt;
	if (elapsedTimeSinceLastWrite >= 2000)
	{
		auto fpms = (stats.PresentCount - presentCount) / elapsedTimeSinceLastWrite;
		auto mspf = 1.0f / fpms;

		// TODO: - camera: ({:.2f}, {:.2f}, {:.2f})
		auto windowText = fmt::format(L"fps: {:.0f} mspf: {:.2f}", 1000.0f * fpms, mspf);
		SetWindowText(Game::Get()->GetWindow()->GetMainWnd(), windowText.c_str());

		elapsedTimeSinceLastWrite = 0;
		presentCount = stats.PresentCount;
	}
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
	// psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT32_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_SwapChain->BackBufferFormat;
	psoDesc.SampleDesc = _MSAA;
	psoDesc.DSVFormat = m_DepthStencil->depthStencilFormat;
	ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));
}

void DX12Engine::SetCameraPosition(CameraPosition3D pos)
{
	m_Camera->UpdateCameraView(pos);
}

void DX12Engine::OnUpdate(milliseconds dt)
{
	static milliseconds t = 0;

	ShowFrameStats(dt);

	auto currFrameRes = _FrameCycle->GetCurrentFrameResource();

	// update main pass

	auto viewProj = m_Camera->GetViewProj();
	currFrameRes->UpdateMainPassConstantBuffers({ viewProj, static_cast<f32>(t) });

	t += dt / 1000.0f;

	// update render items

	for (auto& obj : _Objects->items())
	{
		if (obj.IsDirty())
		{
			currFrameRes->UpdateObjectConstantBuffers(obj.CBIndex, { obj.World });
			obj.Clean();
		}
	}
}

void DX12Engine::OnDraw()
{
	ResetCommandList();

	// Indicate a state transition on the resource usage.
	auto renderTransition = m_SwapChain->GetPresentTransition();
	m_CommandList->ResourceBarrier(1, &renderTransition);

	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	const auto cbbCPUHandle = m_SwapChain->GetCurrentBackBufferCPUHandle();
	const auto dsCPUHandle = m_DepthStencil->GetCPUHandle();

	// Clear the back buffer and depth buffer.
	m_CommandList->ClearRenderTargetView(cbbCPUHandle, Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	m_CommandList->OMSetRenderTargets(1, &cbbCPUHandle, true, &dsCPUHandle);

	auto currFrameRes = _FrameCycle->GetCurrentFrameResource();

	ID3D12DescriptorHeap* descriptorHeaps[] = { currFrameRes->GetDescriptorHeap(), };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_CommandList->SetGraphicsRootSignature(_RootSignature->Get());

	auto gpuHandle = currFrameRes->GetGPUHandle();
	m_CommandList->SetGraphicsRootDescriptorTable(0, currFrameRes->GetGPUHandle());

	for (auto& obj : _Objects->items())
	{
		m_CommandList->IASetVertexBuffers(0, 1, &_Objects->VertexBufferView);
		m_CommandList->IASetIndexBuffer(&_Objects->IndexBufferView);
		m_CommandList->IASetPrimitiveTopology(obj.PrimitiveType);

		// this sets the world matrix in the cbuffer
		gpuHandle.Offset(1, currFrameRes->_cbvHeap.descriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(1, gpuHandle);

		m_CommandList->DrawIndexedInstanced(obj.Submesh->IndexCount, 1, obj.Submesh->StartIndexLocation, obj.Submesh->BaseVertexLocation, 0);
	}

	// Indicate a state transition on the resource usage.
	auto presentTransition = m_SwapChain->GetPresentTransition();
	m_CommandList->ResourceBarrier(1, &presentTransition);

	CloseCommandList();
	ExecuteCommandLists();

	m_SwapChain->Present();

	_FrameCycle->SignalCurrentFrame(m_CommandQueue.Get());
	_FrameCycle->Advance();
}

void DX12Engine::CloseCommandList()
{
	ThrowIfFailed(m_CommandList->Close());
}

void DX12Engine::OnResize(u32 width, u32 height)
{
	if (!IsReady())
		LOG_FATAL("Called OnResize before graphics engine was ready");
	ASSERT(width != NULL && height != NULL);

	ResetCommandList();

	// Release the previous resources we will be recreating.
	m_SwapChain->Resize(m_d3dDevice.Get(), width, height);
	m_DepthStencil->Resize(m_d3dDevice.Get(), width, height, _MSAA);

	auto dsWriteTransition = m_DepthStencil->GetWriteTransition();
	m_CommandList->ResourceBarrier(1, &dsWriteTransition);

	CloseCommandList();
	ExecuteCommandLists();

	m_ScreenViewport = CreateViewport(width, height);
	m_ScissorRect = CreateScissorRectangle(width, height);
	m_Camera->Resize(width, height);

	_FrameCycle->SignalCurrentFrame(m_CommandQueue.Get());
	_FrameCycle->Advance();
}
