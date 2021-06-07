#include "DX12Engine.h"

#include "Common/Pix.h"


#include <GameViewLayer/GraphicsEngines/DX12/Helpers/Shaders/HLSLShaders.h>
#include <Helpers/Colors/Colors.h>

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

	static milliseconds t = 0; // TODO: pass timer
	t += dt / 1000.0f;

	_camera.Update();

	ShowFrameStats(dt);

	auto i = _resources.GetCurrentFrameIndex();

	// render pass
	{
		PassConstants pc = { _camera.GetViewProj(), _camera.GetEyePosition(), static_cast<f32>(t) };
		
		// TODO: make a constant just for this
		pc.numLights = (u32) _lights.size();
		memcpy(pc.lights, _lights.data(), sizeof(Light::Constants) * _lights.size());
		
		auto& rpr = _renderPassResource[i];
		memcpy(rpr.Memory(), &pc, rpr.Size());
	}

}

static const auto DarkGray = FromHex(0x111111);

#pragma region Frame Render
// Draws the scene.
void DX12Engine::OnDraw()
{
	// Prepare the command list to render a new frame.
	_resources.Prepare();
	_resources.Clear(DarkGray);

	auto i = _resources.GetCurrentFrameIndex();
	auto commandList = _resources.GetCommandList();

	// render the mesh
	{
		PixEvent pixRender(DirectX::Colors::Crimson, L"Render");

		commandList->SetGraphicsRootSignature(_rootSignature.Get());
		commandList->SetPipelineState(_pipelineState.Get());

		ID3D12DescriptorHeap* descriptorHeaps[] = { _descriptors->Heap(), };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		for (auto& [name, obj] : _objects)
		{
			commandList->SetGraphicsRootConstantBufferView(PassCB, _renderPassResource[i].GpuAddress());
			commandList->SetGraphicsRootConstantBufferView(ObjectCB, obj->resources[i].GpuAddress());
			commandList->SetGraphicsRootConstantBufferView(MaterialCB, obj->material->resources[i].GpuAddress());

			auto index = obj->texture ? obj->texture->id : Descriptors::None;
			commandList->SetGraphicsRootDescriptorTable(TextureSRV, _descriptors->GetGpuHandle(index));

			obj->model->Draw(commandList);
		}
	}

	// Draw UI
	{
		PixEvent pixPresent(commandList, DirectX::Colors::ForestGreen, L"Draw UI");

		auto size = _resources.GetOutputSize();
		auto safe = Viewport::ComputeTitleSafeArea(size.right, size.bottom);

		_spriteBatch->Begin(commandList);

		Vector2 position((f32)safe.left, (f32)safe.top);
		auto str = std::format("current frame index {}", i);

		_spriteFont->DrawString(_spriteBatch.get(), str.c_str(), position, DarkGray);

		_spriteBatch->End();
	}

	auto cmdQueue = _resources.GetCommandQueue();

	// Show the new frame
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
		slotRootParameters[RootParameterIndex::PassCB].InitAsConstantBufferView(0);
		slotRootParameters[RootParameterIndex::ObjectCB].InitAsConstantBufferView(1);
		slotRootParameters[RootParameterIndex::MaterialCB].InitAsConstantBufferView(2);

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

	// create pipeline state
	{
		RenderTargetState rtState(_resources.GetBackBufferFormat(), _resources.GetDepthBufferFormat());

		EffectPipelineStateDescription pd(
			&GeometricPrimitive::VertexType::InputLayout,
			CommonStates::Opaque,
			CommonStates::DepthDefault,
			CommonStates::CullCounterClockwise,
			rtState);

		HLSLShaders shaders((LPWSTR) Settings::Get("graphics-shader-entrypoint"), _resources.GetShaderModel(), nullptr);

		pd.CreatePipelineState(
			device, _rootSignature.Get(), shaders.GetVSByteCode(), shaders.GetPSByteCode(), &_pipelineState
		);
	}

	ResourceUploadBatch resourceUpload(device);
	resourceUpload.Begin();

	// build up _lights
	_lights.emplace_back(Light::DirectionalDefault);

	// build up _textures
	Texture bricks;
	bricks.id = 1;
	bricks.filename = L"..\\Assets\\Textures\\bricks3.dds";
	_textures["bricks"] = std::make_shared<Texture>(bricks);

	// build up _materials
	_materials["wood"] = std::make_shared<Material::Element>(Material::Wood);
	_materials["ivory"] = std::make_shared<Material::Element>(Material::Ivory);

	// build up _models
	auto geo = GeometricPrimitive::CreateTeapot(4);
	_models["teapot"] = std::move(geo);
	_models["teapot"]->LoadStaticBuffers(device, resourceUpload);

	auto geo2 = GeometricPrimitive::CreateBox({ 100, 100, 100 }, false, true);
	_models["room"] = std::move(geo2);
	_models["room"]->LoadStaticBuffers(device, resourceUpload);

	// build up _objects
	_objects["teapot"] = std::make_shared<Object::Element>();
	_objects["teapot"]->name = "teapot";
	_objects["teapot"]->world = Matrix::Identity;
	_objects["teapot"]->textureTransform = Matrix::Identity;
	_objects["teapot"]->model = _models["teapot"];
	_objects["teapot"]->material = _materials["wood"];
	_objects["teapot"]->texture = _textures["bricks"];

	_objects["teapot2"] = std::make_shared<Object::Element>();
	_objects["teapot2"]->name = "teapot2";
	_objects["teapot2"]->world = Matrix::CreateTranslation({ -5, 0, -5 }) * Matrix::CreateRotationY(1);
	_objects["teapot2"]->textureTransform = Matrix::Identity;
	_objects["teapot2"]->model = _models["teapot"];
	_objects["teapot2"]->material = _materials["wood"];
	_objects["teapot2"]->texture = _textures["bricks"];
	
	_objects["room"] = std::make_shared<Object::Element>();
	_objects["room"]->name = "room";
	_objects["room"]->world = Matrix::CreateTranslation({ 0, 48, 0 });
	_objects["room"]->textureTransform = Matrix::Identity;
	_objects["room"]->model = _models["room"];
	_objects["room"]->material = _materials["ivory"];
	_objects["room"]->texture = _textures["room"];

	// create cbuffer
	{
		for (u32 i = 0; i < _resources.GetBackBufferCount(); i++)
		{
			// add pass
			_renderPassResource[i] = std::move(_graphicsMemory->AllocateConstant<PassConstants>());

			// add materials
			for (auto& [name, material] : _materials)
			{
				auto cbResource = _graphicsMemory->AllocateConstant<Material::Constants>(*material.get());
				material->resources[i] = std::move(cbResource);
			}

			for (auto& [name, object] : _objects)
			{
				auto cbResource = _graphicsMemory->AllocateConstant<Object::Constants>(*object.get());
				object->resources[i] = std::move(cbResource);
			}
		}
	}

	// create texture
	{
		auto index = _textures["bricks"]->id;
		auto filename = _textures["bricks"]->filename.c_str();
		auto& resource = _textures["bricks"]->resource;

		CreateDDSTextureFromFile(device, resourceUpload, filename, resource.ReleaseAndGetAddressOf());

		CreateShaderResourceView(device, resource.Get(), _descriptors->GetCpuHandle(index));
	}

	// create fonts
	{
		RenderTargetState rtStateUI(BACK_BUFFER_FORMAT, DXGI_FORMAT_UNKNOWN);
		SpriteBatchPipelineStateDescription pd(rtStateUI);

		_spriteBatch = std::make_unique<SpriteBatch>(device, resourceUpload, pd);

		_spriteFont = std::make_unique<DirectX::SpriteFont>(device, resourceUpload,
			Settings::Get("graphics-spritefont-filepath"),
			_descriptors->GetCpuHandle(Descriptors::UIFont),
			_descriptors->GetGpuHandle(Descriptors::UIFont));
	}

	resourceUpload.End(_resources.GetCommandQueue());
}

void DX12Engine::CreateWindowSizeDependentResources() {

	auto viewport = _resources.GetScreenViewport();
	_spriteBatch->SetViewport(viewport);

}

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