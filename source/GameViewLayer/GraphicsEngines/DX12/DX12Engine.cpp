#include "DX12Engine.h"

#include "Helpers/DX12Debug.h"
#include "Helpers/DX12DXGI.h"
#include "Helpers/DX12Device.h"
#include "Helpers/DX12MSAA.h"
#include "Helpers/DX12Command.h"
#include "Helpers/DX12Window.h"

#include <GameLogicLayer/Game.h>

#include <vector>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>      
#include <assimp/postprocess.h>
#include <GameViewLayer/GraphicsElements/Material.h>
#include <GameViewLayer/GraphicsElements/Texture.h>


DX12Engine::DX12Engine() :
	_resources(BACK_BUFFER_FORMAT, DEPTH_BUFFER_FORMAT, BACK_BUFFER_COUNT)
{
	_resources.RegisterDeviceNotify(this);

	_MSAA.Count = Settings::GetInt("graphics-msaa-count");
}


DX12Engine::~DX12Engine()
{
	if (IsReady())
		FlushCommandQueue();
}

void DX12Engine::Initialize(HWND window, u16 width, u16 height)
{
#ifdef _DEBUG
	EnableDebugLayer();
#endif

	_resources.SetWindow(window, width, height);
	_resources.CreateDeviceResources();
	// _resources.CreateWindowSizeDependentResources();

	auto factory = _resources.GetDXGIFactory();
	auto device = _resources.GetD3DDevice(); 
	auto cmdQueue = _resources.GetCommandQueue();

	_SwapChain = std::make_unique<SwapChainManager>(factory, device, cmdQueue);
	_DepthStencil = std::make_unique<DepthStencilManager>(device);

	_RootSignature = std::make_unique<RootSignature>(device, 3); // TODO: set this in a define I think
	_Shaders = std::make_unique<HLSLShaders>((wchar_t*)Settings::Get("graphics-shader-entrypoint"), nullptr);
	_FrameCycle = std::make_unique<FrameCycle>(device, 30, 3); // TODO: this should come from somewhere else
	_Camera = std::make_unique<Camera>();

	Command::CreateList(device, _FrameCycle->GetCurrentFrameAllocatorWhenAvailable(), m_CommandList.GetAddressOf());
	m_CommandList->SetName(L"Main");

	_MSAA = MSAA::Check(device, _SwapChain->BackBufferFormat, _MSAA.Count, _MSAA.Quality);
	LOG_INFO(fmt::format("Using MSAA with {} samples and {} quality levels", _MSAA.Count, _MSAA.Quality));

	// DirectX Tool Kit 12
	_resourceDescriptors = std::make_unique<DescriptorHeap>(device, Descriptors::Count);

#ifdef _DEBUG
	Display::LogInformation(factory, _SwapChain->BackBufferFormat);
#endif

	CloseCommandList(); // The command list must be closed before passing it off to the GPU.
	ResetCommandList(); // Reset the command list to prep for initialization commands.

	BuildPipelineStateObject();
	BuildGeometry();

	CloseCommandList();
	ExecuteCommandLists();
	SignalFrameAndAdvance();

	OnResize(width, height);

	m_CommandList = _resources.GetCommandList();
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
	_resources.GetCommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

void DX12Engine::FlushCommandQueue()
{
	_FrameCycle->Flush(_resources.GetCommandQueue());
}

void DX12Engine::BuildGeometry()
{
	// TODO: after learning materials and lights, make a proper importer in the GameApp resource loader
	//Assimp::Importer importer;

	//const aiScene* scene = importer.ReadFile("mug.dae",
	//	aiProcess_Triangulate |
	//	aiProcess_JoinIdenticalVertices |
	//	aiProcess_SortByPType
	//	// aiProcess_MakeLeftHanded
	//);

	//if (!scene)
	//	LOG_ERROR(importer.GetErrorString());

	//Mesh mug = {};

	//mug.Vertices.resize(scene->mMeshes[0]->mNumVertices);
	//auto vBegin = std::make_move_iterator(scene->mMeshes[0]->mVertices);
	//auto vEnd = std::make_move_iterator(scene->mMeshes[0]->mVertices + scene->mMeshes[0]->mNumVertices);
	//std::transform(vBegin, vEnd, mug.Vertices.begin(), [] (aiVector3D v) { return Vertex(v); });

	//for (u32 i = 0; i < scene->mMeshes[0]->mNumFaces; i++)
	//	for (u32 j = 0; j < 3; j++)
	//		mug.Indices.push_back(scene->mMeshes[0]->mFaces[i].mIndices[j]);

	auto device = _resources.GetD3DDevice();

	Material woodCrate;
	woodCrate.Name = "woodCrate";
	woodCrate.MatCBIndex = 0;
	woodCrate.DiffuseSRVHeapIndex = 0;
	woodCrate.DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	woodCrate.FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	woodCrate.Roughness = 0.2f;
	XMStoreFloat4x4(&woodCrate.Transform, XMMatrixIdentity());

	Texture tex;
	tex.Name = "fenceTex";
	tex.Filename = L"Textures\\bricks3.dds";

	ResourceUploadBatch resourceUpload(device);

	resourceUpload.Begin();

	CreateDDSTextureFromFile(
		device,
		resourceUpload,
		tex.Filename.c_str(), 
		tex.Resource.GetAddressOf()
	);

	resourceUpload.End(_resources.GetCommandQueue());

	CreateShaderResourceView(
		device,
		tex.Resource.Get(),
		_resourceDescriptors->GetCpuHandle(Descriptors::BrickTexture)
	);

	Mesh m;
	GeometricPrimitive::CreateBox(m.Vertices, m.Indices, { 4, 4, 4 });

	// auto m = GeometricPrimitive::CreateBox({ 3, 3, 3 });

	RenderObjects::MeshMap staticMeshes = {
		{ "m", &m }, // TODO: fix empty static mesh breaking
	};

	RenderObjects::MeshMap dynamicMeshes = {
		// { "sphere", &_Sphere }
	};

	RenderObjects::Objects objs = {
		{ "m", XMMatrixIdentity(), XMMatrixIdentity(), woodCrate, tex },
	};

	/*
	TODO: instead of passing three objects, try something like this
	
	auto origin = XMMatrixIdentity();
	auto x = {
		{ "box", &origin, &box, &material, &texture },
	}
	*/

	_Objects = std::make_unique<RenderObjects>(staticMeshes, dynamicMeshes, objs, device, m_CommandList.Get());
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
	psoDesc.InputLayout = Mesh::Vertex::InputLayout;
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

	auto device = _resources.GetD3DDevice();
	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));
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

	currFrameRes->UpdateMainPassConstantBuffers({ 
		_Camera->GetViewProj(), 
		_Camera->GetEyePosition(), 
		static_cast<f32>(t),
		static_cast<f32>(dt),
	});

	// update render items and materials

	auto& items = _Objects->items();
	for (auto i = 0; i < items.size(); i++)
	{
		auto& obj = items[i];
		if (obj.IsDirty())
		{
			currFrameRes->UpdateObjectConstantBuffers(i, { obj.World, obj.TexTransform });
			obj.Clean();
		}

		if (obj.Material->IsDirty())
		{
			currFrameRes->UpdateMaterialConstantBuffers(obj.Material->MatCBIndex, obj.Material->GetConstants());
			obj.Material->Clean();
		}
	}

	// auto newMesh = wavesActor.Update(dt);
	
	/*auto dp = sin(10 * t);
	Mesh newMesh = _Sphere;
	for (auto& v : newMesh.Vertices)
	{
		auto newPos = XMLoadFloat3(&v.Position) + XMVectorSet(0, dp, 0, 0);
		XMStoreFloat3(&v.Position, newPos);
	}

	_Objects->UpdateMesh("sphere", currFrameRes->Index, &newMesh);*/
	
}

void DX12Engine::OnDraw()
{
	ResetCommandList();

	// Indicate a state transition on the resource usage.
	auto t1 = _SwapChain->GetRenderTransition();
	m_CommandList->ResourceBarrier(1, &t1);

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

	ID3D12DescriptorHeap* descriptorHeaps[] = { _resourceDescriptors->Heap(), };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_CommandList->SetGraphicsRootSignature(_RootSignature->Get());

	auto objCBV = currFrameRes->_ObjectCB.GetBufferView();
	auto matCBV = currFrameRes->_MaterialCB.GetBufferView();
	for (auto& obj : _Objects->items())
	{
		auto vbv = obj.GetVertexBufferView();
		m_CommandList->IASetVertexBuffers(0, 1, &vbv);

		auto ibv = obj.GetIndexBufferView();
		m_CommandList->IASetIndexBuffer(&ibv);

		m_CommandList->IASetPrimitiveTopology(obj.PrimitiveType);

		// tex.Offset(ri->Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(0, _resourceDescriptors->GetGpuHandle(Descriptors::BrickTexture));

		// this sets the world matrix in the cbuffer
		m_CommandList->SetGraphicsRootConstantBufferView(1, objCBV.BufferLocation);
		objCBV.BufferLocation += objCBV.ElementByteSize;

		m_CommandList->SetGraphicsRootConstantBufferView(3, matCBV.BufferLocation + (u64) obj.Material->MatCBIndex * matCBV.ElementByteSize);

		m_CommandList->DrawIndexedInstanced(obj.Submesh.IndexCount, 1, obj.Submesh.StartIndexLocation, obj.Submesh.BaseVertexLocation, 0);
	}

	m_CommandList->SetGraphicsRootConstantBufferView(2, currFrameRes->_PassCB.GetBufferView().BufferLocation);

	// Indicate a state transition on the resource usage.
	auto t2 = _SwapChain->GetPresentTransition();
	m_CommandList->ResourceBarrier(1, &t2);

	CloseCommandList();
	ExecuteCommandLists();

	_SwapChain->Present();

	SignalFrameAndAdvance();
}

void DX12Engine::CloseCommandList()
{
	ThrowIfFailed(m_CommandList->Close());
}

void DX12Engine::OnResize(u16 width, u16 height)
{
	if (!IsReady())
		LOG_FATAL("Called OnResize before graphics engine was ready");
	ASSERT(width != NULL && height != NULL);

	auto device = _resources.GetD3DDevice();

	// Release the previous resources we will be recreating.
	_SwapChain->Resize(device, width, height);
	_DepthStencil->Resize(device, width, height);

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
	_FrameCycle->SignalCurrentFrame(_resources.GetCommandQueue());
	_FrameCycle->Advance();
}

void DX12Engine::OnDeviceLost()
{
	_resourceDescriptors.reset();
	_graphicsMemory.reset();

	_Camera.reset();
}

void DX12Engine::OnDeviceRestored()
{
	throw std::exception("not ready");
	// TODO
	// CreateDeviceDependentResources();
	// CreateWindowSizeDependentResources();
}