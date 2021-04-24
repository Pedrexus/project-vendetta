#include "DX12Engine.h"

#include "Helpers/DX12Debug.h"
#include "Helpers/DX12DXGI.h"
#include "Helpers/DX12Device.h"
#include "Helpers/DX12MSAA.h"
#include "Helpers/DX12Command.h"
#include "Helpers/DX12Window.h"
#include "Helpers/Buffers/DefaultBuffer.h"
#include "Helpers/InputAssembler/Vertex.h"
#include "Helpers/Objects/Geometry.h"

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
	
	CheckMSAASupport();
	CreateCommandObjects();
	
	m_SwapChain = std::make_unique<SwapChainManager>(m_dxgiFactory.Get(), m_d3dDevice.Get(), m_CommandQueue.Get(), m_msaa);
	m_DepthStencil = std::make_unique<DepthStencilManager>(m_d3dDevice.Get());

#ifdef _DEBUG
	Display::LogInformation(m_dxgiFactory.Get(), m_SwapChain->BackBufferFormat);
#endif

	// Reset the command list to prep for initialization commands.
	ResetCommandList();

	BuildPipelineStateObject();
	BuildBoxGeometry();

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
	Command::CreateList(m_d3dDevice.Get(), _FrameCycle->GetCurrentFrameAllocatorWhenAvailable(), m_CommandList.GetAddressOf());
	m_CommandList->SetName(L"Main");

	// The command list must be closed before passing it off to the GPU.
	CloseCommandList();
}

void DX12Engine::BuildBoxGeometry()
{
	auto box = Geometry::CreateBox(0, 0, 0);
	auto grid = Geometry::CreateGrid(10, 10, 40, 40);
	auto sphere = Geometry::CreateGeosphere(.8f, 3);
	auto cylinder = Geometry::CreateCylinder(.8f, .4f, 1.0f, 20, 20);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT) box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT) grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT) sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT) box.Indices.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT) grid.Indices.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT) sphere.Indices.size();

	// Define the SubmeshGeometry that cover different 
	// regions of the vertex/index buffers.

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT) box.Indices.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT) grid.Indices.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT) sphere.Indices.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT) cylinder.Indices.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount = box.Vertices.size() + grid.Vertices.size() + sphere.Vertices.size() + cylinder.Vertices.size();
	auto totalIndexCount = box.Indices.size() + grid.Indices.size() + sphere.Indices.size() + cylinder.Indices.size();;
	std::vector<Mesh> meshArray = { box, grid, sphere, cylinder };
	
	std::vector<Vertex> vertices;
	vertices.reserve(totalVertexCount);
	std::vector<u16> indices;
	indices.reserve(totalIndexCount);

	std::for_each(meshArray.begin(), meshArray.end(),
		[&vertices, &indices] (Mesh mesh) {
			vertices.insert(vertices.end(), std::make_move_iterator(mesh.Vertices.begin()), std::make_move_iterator(mesh.Vertices.end()));

			auto meshIndices = mesh.GetIndices<u16>();
			indices.insert(indices.end(), std::make_move_iterator(meshIndices.begin()), std::make_move_iterator(meshIndices.end()));
		}
	);

	const UINT vbByteSize = (UINT) vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT) indices.size() * sizeof(u16);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = std::make_shared<DefaultBuffer>(m_d3dDevice.Get(),
		m_CommandList.Get(), vertices.data(), vbByteSize);

	geo->IndexBufferGPU = std::make_shared<DefaultBuffer>(m_d3dDevice.Get(),
		m_CommandList.Get(), indices.data(), ibByteSize);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	_Geometries[geo->Name] = std::move(geo);

	// Build Render Items

	auto boxRitem = std::make_unique<RenderItem>();
	boxRitem->World = XMMatrixTranslation(1.0f, 0.5f, 1.0f);
	boxRitem->ObjCBIndex = 0;
	boxRitem->Geo = _Geometries["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	_AllRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = XMMatrixIdentity();
	gridRitem->ObjCBIndex = 1;
	gridRitem->Geo = _Geometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	_AllRitems.push_back(std::move(gridRitem));

	UINT objCBIndex = 2;
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		leftCylRitem->World = rightCylWorld;
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Geo = _Geometries["shapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		rightCylRitem->World = leftCylWorld;
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Geo = _Geometries["shapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		leftSphereRitem->World = leftSphereWorld;
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Geo = _Geometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		rightSphereRitem->World = rightSphereWorld;
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Geo = _Geometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		_AllRitems.push_back(std::move(leftCylRitem));
		_AllRitems.push_back(std::move(rightCylRitem));
		_AllRitems.push_back(std::move(leftSphereRitem));
		_AllRitems.push_back(std::move(rightSphereRitem));
	}
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
	psoDesc.SampleDesc = m_msaa;
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
	
	for (auto& e : _AllRitems)
	{
		if (e->IsDirty())
		{
			currFrameRes->UpdateObjectConstantBuffers(e->ObjCBIndex, { e->World });
			e->Clean();
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

	for (auto& e: _AllRitems)
	{
		auto vertexView = e->Geo->VertexBufferView();
		auto indexView = e->Geo->IndexBufferView();

		m_CommandList->IASetVertexBuffers(0, 1, &vertexView);
		m_CommandList->IASetIndexBuffer(&indexView);
		m_CommandList->IASetPrimitiveTopology(e->PrimitiveType);

		gpuHandle.Offset(1, currFrameRes->_cbvHeap.descriptorSize); // objCBIndex is unnecessary
		m_CommandList->SetGraphicsRootDescriptorTable(1, gpuHandle);

		m_CommandList->DrawIndexedInstanced(e->IndexCount, 1, e->StartIndexLocation, e->BaseVertexLocation, 0);
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
	m_DepthStencil->Resize(m_d3dDevice.Get(), width, height, m_msaa);

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
