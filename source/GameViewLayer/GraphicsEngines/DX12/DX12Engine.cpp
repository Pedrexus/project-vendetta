#include "DX12Engine.h"

#include "Common/Pix.h"
#include <GameViewLayer/GraphicsEngines/DX12/Helpers/Shaders/HLSLShaders.h>

DX12Engine::DX12Engine() :
	_camera(),
	_resources(BACK_BUFFER_FORMAT, DEPTH_BUFFER_FORMAT, BACK_BUFFER_COUNT, D3D_FEATURE_LEVEL_11_0, DX::DeviceResources::c_AllowTearing)
{
	_resources.RegisterDeviceNotify(this);
}

DX12Engine::~DX12Engine()
{
	_resources.WaitForGpu();
}


// Initialize the Direct3D resources required to run.
void DX12Engine::Initialize(HWND window, u16 width, u16 height)
{
	_camera.Resize(width, height);
	_resources.SetWindow(window, width, height);

	_resources.CreateDeviceResources();
	CreateDeviceDependentResources();

	_resources.CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	_resources.WaitForGpu();
}

// TODO: update to use step_timer class
void DX12Engine::OnUpdate(milliseconds dt)
{
	PixEvent e(DirectX::Colors::AliceBlue, L"Update");

	static milliseconds t = 0;
	t += dt / 1000.0f;

	ShowFrameStats(dt);

	auto i = _resources.GetCurrentFrameIndex();

	// render pass
	{
		PassConstants pc = { _camera.GetViewProj() };
		auto& rpr = m_renderPassResource[i];
		memcpy(rpr.Memory(), &pc, rpr.Size());
	}

	// object constants
	/*{
		ObjectConstants oc = { Matrix::CreateRotationZ(t) * Matrix::CreateRotationX(t) };
		auto& res = m_constantBufferResource[i];
		memcpy(res.Memory(), &oc, res.Size());
	}*/
}


#pragma region Frame Render
// Draws the scene.
void DX12Engine::OnDraw()
{
	// Prepare the command list to render a new frame.
	_resources.Prepare();
	_resources.Clear(Colors::DimGray);

	auto i = _resources.GetCurrentFrameIndex();
	auto commandList = _resources.GetCommandList();

	// render the mesh
	{
		PixEvent pixRender(DirectX::Colors::Crimson, L"Render");

		commandList->SetGraphicsRootSignature(_rootSignature.Get());
		commandList->SetPipelineState(_pipelineState.Get());

		ID3D12DescriptorHeap* descriptorHeaps[] = { _descriptors->Heap(), };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// Set necessary state.
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		commandList->IASetIndexBuffer(&m_indexBufferView);

		commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::PassConstant, m_renderPassResource[i].GpuAddress());
		commandList->SetGraphicsRootConstantBufferView(RootParameterIndex::ObjectConstant, m_constantBufferResource[i].GpuAddress());
		commandList->SetGraphicsRootDescriptorTable(RootParameterIndex::TextureSRV, _descriptors->GetGpuHandle(Descriptors::BrickTexture));

		// Draw
		commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
	}

	auto cmdQueue = _resources.GetCommandQueue();

	// Show the new frame.
	{
		PixEvent pixPresent(cmdQueue, DirectX::Colors::Teal, L"Present");

		_resources.Present();
		_graphicsMemory->Commit(cmdQueue);
	}

}
#pragma endregion

void DX12Engine::OnResize(u16 width, u16 height)
{
	_camera.Resize(width, height);
	_resources.WindowSizeChanged(width, height);
}

#pragma region Direct3D Resources
// Allocate all memory resources that change on a window SizeChanged event.

void DX12Engine::CreateDeviceDependentResources()
{
	auto device = _resources.GetD3DDevice();

	_graphicsMemory = std::make_unique<DirectX::GraphicsMemory>(device);
	_descriptors = std::make_unique<DirectX::DescriptorHeap>(device, Descriptors::Count);

	// Create an 1 buffer root signature.
	{
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_PARAMETER slotRootParameters[RootParameterIndex::RootParameterCount] = {};
		slotRootParameters[RootParameterIndex::PassConstant].InitAsConstantBufferView(0);
		slotRootParameters[RootParameterIndex::ObjectConstant].InitAsConstantBufferView(1);

		// textures (t0, ..., tn)
		CD3DX12_DESCRIPTOR_RANGE textureSRV(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		slotRootParameters[RootParameterIndex::TextureSRV].InitAsDescriptorTable(1, &textureSRV, D3D12_SHADER_VISIBILITY_PIXEL);

		D3D12_STATIC_SAMPLER_DESC samplers[2] = { CommonStates::StaticLinearClamp(0), CommonStates::StaticAnisotropicWrap(1) };

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
			RootParameterIndex::RootParameterCount, slotRootParameters, _countof(samplers), samplers, rootSignatureFlags
		);

		DX::ThrowIfFailed(
			CreateRootSignature(device, &rootSignatureDesc, _rootSignature.ReleaseAndGetAddressOf())
		);
	}

	RenderTargetState rtState(_resources.GetBackBufferFormat(), _resources.GetDepthBufferFormat());

	EffectPipelineStateDescription pd(
		&GeometricPrimitive::VertexType::InputLayout,
		CommonStates::Opaque,
		CommonStates::DepthDefault,
		CommonStates::CullCounterClockwise,
		rtState);

	HLSLShaders shaders((LPWSTR) Settings::Get("graphics-shader-entrypoint"), _resources.GetShaderFeatureData(), nullptr);

	pd.CreatePipelineState(
		device, _rootSignature.Get(), shaders.GetVSByteCode(), shaders.GetPSByteCode(), &_pipelineState
	);

	std::vector<GeometricPrimitive::VertexType> vertices;
	std::vector<UINT16> indices;
	GeometricPrimitive::CreateBox(vertices, indices, { 4, 4, 4 });

	ResourceUploadBatch resourceUpload(device);
	resourceUpload.Begin();

	/*auto box = GeometricPrimitive::CreateBox({ 4, 4, 4 });
	box->LoadStaticBuffers(device, resourceUpload);*/

	// Create vertex buffer.
	{
		DX::ThrowIfFailed(
			CreateStaticBuffer(device, resourceUpload, vertices, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_vertexBuffer)
		);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(GeometricPrimitive::VertexType);
		m_vertexBufferView.SizeInBytes = vertices.size() * sizeof(GeometricPrimitive::VertexType);
	}

	// Create index buffer.
	{
		DX::ThrowIfFailed(
			CreateStaticBuffer(device, resourceUpload, indices, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_indexBuffer)
		);

		// Initialize the vertex buffer view.
		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		m_indexBufferView.SizeInBytes = indices.size() * sizeof(UINT16);
	}

	// create cbuffer
	{
		PassConstants pc = { _camera.GetViewProj() };
		for (auto i = 0; i < _resources.GetBackBufferCount(); i++)
			m_renderPassResource.push_back(std::move(_graphicsMemory->AllocateConstant(pc)));


		ObjectConstants oc = { Matrix::Identity };
		for (auto i = 0; i < _resources.GetBackBufferCount(); i++)
			m_constantBufferResource.push_back(std::move(_graphicsMemory->AllocateConstant(oc)));
	}

	// create texture
	{
		auto filename = L"Textures\\bricks3.dds";

		CreateDDSTextureFromFile(device, resourceUpload, filename,
			m_textureResource.ReleaseAndGetAddressOf()
		);

		CreateShaderResourceView(device, m_textureResource.Get(),
			_descriptors->GetCpuHandle(Descriptors::BrickTexture)
		);
	}

	resourceUpload.End(_resources.GetCommandQueue());
}

void DX12Engine::CreateWindowSizeDependentResources() {}

void DX12Engine::OnDeviceLost()
{
	_descriptors.reset();
	_graphicsMemory.reset();
}

void DX12Engine::OnDeviceRestored()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
#pragma endregion

#pragma region Extras
void DX12Engine::SetCameraPosition(CameraPosition3D pos)
{
	_camera.UpdateCameraView(pos);
}

void DX12Engine::ShowFrameStats(milliseconds& dt)
{
	static auto presentCount = 0;
	static f64 elapsedTimeSinceLastWrite = 0;

	DXGI_FRAME_STATISTICS stats;
	_resources.GetSwapChain()->GetFrameStatistics(&stats);

	elapsedTimeSinceLastWrite += dt;
	if (elapsedTimeSinceLastWrite >= 2000)
	{
		auto fpms = (stats.PresentCount - presentCount) / elapsedTimeSinceLastWrite;
		auto mspf = 1.0f / fpms;

		// TODO: - camera: ({:.2f}, {:.2f}, {:.2f})
		auto windowText = std::format(L"fps: {:.0f} mspf: {:.2f}", 1000.0f * fpms, mspf);
		SetWindowText(_resources.GetWindow(), windowText.c_str());

		elapsedTimeSinceLastWrite = 0;
		presentCount = stats.PresentCount;
	}
}
#pragma endregion