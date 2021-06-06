#pragma once

#include <types.h>

#include <Helpers/Settings/Settings.h>

#include "Common/DeviceResources.h"
#include "Common/Elements.h"

#include "Helpers/Camera/Camera.h"

#include "../IGraphicsEngine.h"

namespace // engine constants
{
	static constexpr auto BACK_BUFFER_FORMAT = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	static const auto BACK_BUFFER_COUNT = Settings::GetInt("graphics-frame-resources");

	static constexpr auto DEPTH_BUFFER_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT; // DXGI_FORMAT_D32_FLOAT; 
	static constexpr auto MSAA_DEPTH_BUFFER_FORMAT = DXGI_FORMAT_UNKNOWN; /* If we were only doing MSAA rendering, we could skip the non-MSAA depth/stencil buffer with DXGI_FORMAT_UNKNOWN */

	static const auto MSAA_SAMPLE_COUNT = Settings::GetInt("graphics-msaa-count");
}


struct alignas(16) PassConstants
{
	Matrix viewProj;
	Vector3 eyePosition;
	f32 time;

	u32 numLights;
	Light::Constants lights[Light::MAXIMUM_AMOUNT];
};

class DX12Engine : public IGraphicsEngine, public DX::IDeviceNotify
{
	Camera _camera;

	DX::DeviceResources _resources;

	// DirectX Tool Kit 12
	std::unique_ptr<DirectX::DescriptorHeap>		_descriptors;
	std::unique_ptr<DirectX::GraphicsMemory>		_graphicsMemory;

	Microsoft::WRL::ComPtr<ID3D12RootSignature>		_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		_pipelineState;

	DirectX::GraphicsResource						m_renderPassResource[MAX_BACK_BUFFER_COUNT];

	// referenced by object
	DirectX::GraphicsResource						m_objectResource[MAX_BACK_BUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D12Resource>			m_textureResource;
	DirectX::GraphicsResource						m_materialResource[MAX_BACK_BUFFER_COUNT];

	// world for the graphics engine
	std::unordered_map<std::string, std::shared_ptr<Texture>> _textures;

	std::unordered_map<std::string, std::shared_ptr<Material::Element>> _materials;
	std::unordered_map<std::string, std::shared_ptr<GeometricPrimitive>> _models;
	std::unordered_map<std::string, std::shared_ptr<Object::Element>> _objects;
	
	std::vector<Light::Constants> _lights;

	enum RootParameterIndex
	{
		TextureSRV,
		PassCB,
		ObjectCB,
		MaterialCB,
		RootParameterCount
	};

	enum Descriptors
	{
		None,
		BrickTexture,
		Count
	};

protected:
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

public:
	DX12Engine() noexcept(false);
	~DX12Engine();

	// IGraphicsEngine
	void Initialize(HWND window, u16 width, u16 height) override;
	inline bool IsReady() override { return _resources.GetD3DDevice() && _resources.GetSwapChain(); };

	void OnUpdate(milliseconds dt) override;
	void OnDraw() override;
	void OnResize(u16 width = NULL, u16 height = NULL) override;

	// IDeviceNotify
	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	void ShowFrameStats(milliseconds& dt); // TODO: use timer

	// no copy, no move
	DX12Engine(DX12Engine& rhs) = delete;
	DX12Engine(const DX12Engine& rhs) = delete;
	DX12Engine& operator=(DX12Engine& rhs) = delete;
	DX12Engine& operator=(const DX12Engine& rhs) = delete;
};