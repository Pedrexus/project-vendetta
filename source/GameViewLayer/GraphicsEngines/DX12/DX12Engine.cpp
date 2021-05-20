#include "DX12Engine.h"

#include "Helpers/DX12Debug.h"
#include "Helpers/DX12DXGI.h"
#include "Helpers/DX12Device.h"
#include "Helpers/DX12MSAA.h"
#include "Helpers/DX12Command.h"
#include "Helpers/DX12Window.h"
#include "Helpers/InputAssembler/Vertex.h"
#include "Helpers/InputAssembler/Objects/Geometry.h"

#include <GameLogicLayer/Game.h>

#include <vector>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>      
#include <assimp/postprocess.h>


DX12Engine::~DX12Engine()
{
	if (_Device)
		FlushCommandQueue();
}

void DX12Engine::Initialize()
{
#ifdef _DEBUG
	EnableDebugLayer();
#endif

	// TODO: split into Constructor and initialize ?
	_Factory = DXGI::Factory::CreateWithDebugLayer();
	_Device = CreateHardwareDeviceWithHighestPerformanceAdapterAvailable(_Factory.Get());

	_Camera = std::make_unique<Camera>();

	Command::CreateQueue(_Device.Get(), m_CommandQueue.GetAddressOf());

	_SwapChain = std::make_unique<SwapChainManager>(_Factory.Get(), _Device.Get(), m_CommandQueue.Get());
	_DepthStencil = std::make_unique<DepthStencilManager>(_Device.Get());

	_RootSignature = std::make_unique<RootSignature>(_Device.Get(), 2);
	_Shaders = std::make_unique<HLSLShaders>(L"Shaders\\color.hlsl", nullptr);
	_FrameCycle = std::make_unique<FrameCycle>(_Device.Get(), 30);

	Command::CreateList(_Device.Get(), _FrameCycle->GetCurrentFrameAllocatorWhenAvailable(), m_CommandList.GetAddressOf());
	m_CommandList->SetName(L"Main");

	_MSAA = MSAA::Check(_Device.Get(), _SwapChain->BackBufferFormat, _MSAA.Count, _MSAA.Quality);
	LOG_INFO(fmt::format("Using MSAA with {} samples and {} quality levels", _MSAA.Count, _MSAA.Quality));

#ifdef _DEBUG
	Display::LogInformation(_Factory.Get(), _SwapChain->BackBufferFormat);
#endif

	CloseCommandList(); // The command list must be closed before passing it off to the GPU.
	ResetCommandList(); // Reset the command list to prep for initialization commands.

	BuildPipelineStateObject();
	BuildGeometry();

	CloseCommandList();
	ExecuteCommandLists();
	SignalFrameAndAdvance();
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

void DX12Engine::BuildGeometry()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("mug.dae",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_MakeLeftHanded
	);

	if (!scene)
		LOG_ERROR(importer.GetErrorString());

	LOG_INFO("number of meshes: {}", scene->mNumMeshes);

	Mesh mug = {};

	mug.Vertices.resize(scene->mMeshes[0]->mNumVertices);
	auto vBegin = std::make_move_iterator(scene->mMeshes[0]->mVertices);
	auto vEnd = std::make_move_iterator(scene->mMeshes[0]->mVertices + scene->mMeshes[0]->mNumVertices);
	std::transform(vBegin, vEnd, mug.Vertices.begin(), [] (aiVector3D v) { return Vertex(v); });

	for (auto i = 0; i < scene->mMeshes[0]->mNumFaces; i++)
		for (auto j = 0; j < 3; j++)
			mug.Indices.push_back(scene->mMeshes[0]->mFaces[i].mIndices[j]);

	_Sphere = Geometry::CreateIcosahedron();

	RenderObjects::MeshMap staticMeshes = {
		{ "mug", &mug },
	};

	RenderObjects::MeshMap dynamicMeshes = {
		// { "sphere", &_Sphere }
	};

	RenderObjects::WorldMap positions = {
		{ "mug", XMMatrixIdentity() },
	};

	_Objects = std::make_unique<RenderObjects>(staticMeshes, dynamicMeshes, positions, _Device.Get(), m_CommandList.Get());
}

void DX12Engine::ShowFrameStats(milliseconds& dt)
{
	static auto presentCount = 0;
	static f64 elapsedTimeSinceLastWrite = 0;

	auto stats = _SwapChain->GetFrameStatistics();

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
	psoDesc.RTVFormats[0] = _SwapChain->BackBufferFormat;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.DSVFormat = _DepthStencil->depthStencilFormat;
	ThrowIfFailed(_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));
}

void DX12Engine::SetCameraPosition(CameraPosition3D pos)
{
	_Camera->UpdateCameraView(pos);
}

void DX12Engine::OnUpdate(milliseconds dt)
{
	static milliseconds t = 0;
	t += dt / 1000.0f;

	ShowFrameStats(dt);

	auto currFrameRes = _FrameCycle->GetCurrentFrameResource();

	// update main pass

	auto viewProj = _Camera->GetViewProj();
	currFrameRes->UpdateMainPassConstantBuffers({ viewProj, static_cast<f32>(t) });

	// update render items

	auto& items = _Objects->items();
	for (auto i = 0; i < items.size(); i++)
	{
		auto& obj = items[i];
		if (obj.IsDirty())
		{
			currFrameRes->UpdateObjectConstantBuffers(i, { obj.World });
			obj.Clean();
		}
	}

	// auto newMesh = wavesActor.Update(dt);
	/*
	auto dp = sin(10 * t);
	Mesh newMesh = _Sphere;
	for (auto& v : newMesh.Vertices)
	{
		auto newPos = XMLoadFloat3(&v.Position) + XMVectorSet(0, dp, 0, 0);
		XMStoreFloat3(&v.Position, newPos);
	}

	_Objects->UpdateMesh("sphere", currFrameRes->Index, &newMesh);
	*/
}

void DX12Engine::OnDraw()
{
	ResetCommandList();

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, _SwapChain->GetRenderTransition());

	m_CommandList->RSSetViewports(1, &_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &_ScissorRect);

	const auto cbbCPUHandle = _SwapChain->GetCurrentBackBufferCPUHandle();
	const auto dsCPUHandle = _DepthStencil->GetCPUHandle();

	// Clear the back buffer and depth buffer.
	m_CommandList->ClearRenderTargetView(cbbCPUHandle, Colors::LightSteelBlue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	m_CommandList->OMSetRenderTargets(1, &cbbCPUHandle, true, &dsCPUHandle);

	auto currFrameRes = _FrameCycle->GetCurrentFrameResource();

	ID3D12DescriptorHeap* descriptorHeaps[] = { currFrameRes->GetDescriptorHeap(), };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_CommandList->SetGraphicsRootSignature(_RootSignature->Get());

	auto objCBV = currFrameRes->_ObjectCB.GetBufferView();
	for (auto& obj : _Objects->items())
	{
		// this sets the world matrix in the cbuffer
		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBV.BufferLocation);
		objCBV.BufferLocation += objCBV.ElementByteSize;

		auto vbv = obj.GetVertexBufferView();
		m_CommandList->IASetVertexBuffers(0, 1, &vbv);

		auto ibv = obj.GetIndexBufferView();
		m_CommandList->IASetIndexBuffer(&ibv);

		m_CommandList->IASetPrimitiveTopology(obj.PrimitiveType);
		m_CommandList->DrawIndexedInstanced(obj.Submesh.IndexCount, 1, obj.Submesh.StartIndexLocation, obj.Submesh.BaseVertexLocation, 0);
	}

	m_CommandList->SetGraphicsRootConstantBufferView(1, currFrameRes->_PassCB.GetBufferView().BufferLocation);

	// Indicate a state transition on the resource usage.
	m_CommandList->ResourceBarrier(1, _SwapChain->GetPresentTransition());

	CloseCommandList();
	ExecuteCommandLists();

	_SwapChain->Present();

	SignalFrameAndAdvance();
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

	// Release the previous resources we will be recreating.
	_SwapChain->Resize(_Device.Get(), width, height);
	_DepthStencil->Resize(_Device.Get(), width, height);

	ResetCommandList();

	m_CommandList->ResourceBarrier(1, _DepthStencil->GetWriteTransition());

	CloseCommandList();
	ExecuteCommandLists();

	_ScreenViewport = CreateViewport(width, height);
	_ScissorRect = CreateScissorRectangle(width, height);
	_Camera->Resize(width, height);

	SignalFrameAndAdvance();
}

void DX12Engine::SignalFrameAndAdvance()
{
	_FrameCycle->SignalCurrentFrame(m_CommandQueue.Get());
	_FrameCycle->Advance();
}
