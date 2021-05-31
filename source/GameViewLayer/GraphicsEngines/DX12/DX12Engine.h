#pragma once

#include <types.h>

#include <Helpers/Settings/Settings.h>

#include "Common/DeviceResources.h"
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
};

struct alignas(16) ObjectConstants
{
	Matrix world;
};


class DX12Engine : public IGraphicsEngine, public DX::IDeviceNotify
{
	Camera _camera;

	DX::DeviceResources _resources; // TODO: test if making this a pointer improves performance

	// DirectX Tool Kit 12
	std::unique_ptr<DirectX::DescriptorHeap>		_descriptors;
	std::unique_ptr<DirectX::GraphicsMemory>		_graphicsMemory;

	Microsoft::WRL::ComPtr<ID3D12RootSignature>		_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		_pipelineState;

	Microsoft::WRL::ComPtr<ID3D12Resource>			m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW						m_vertexBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource>			m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW							m_indexBufferView;

	std::vector<DirectX::GraphicsResource>			m_renderPassResource;
	std::vector<DirectX::GraphicsResource>			m_constantBufferResource;

	Microsoft::WRL::ComPtr<ID3D12Resource>			m_textureResource;

	enum RootParameterIndex
	{
		PassConstant,
		ObjectConstant,
		TextureSRV,
		// TextureSampler,
		RootParameterCount
	};

	enum Descriptors
	{
		BrickTexture,
		Count
	};

protected:
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

public:
	DX12Engine() noexcept(false);
	~DX12Engine();

	void Initialize(HWND window, u16 width, u16 height) override;
	inline bool IsReady() override { return _resources.GetD3DDevice() && _resources.GetSwapChain(); };

	// IDeviceNotify
	void OnDeviceLost() override;
	void OnDeviceRestored() override;
	void OnUpdate(milliseconds dt) override;
	void OnDraw() override;
	void OnResize(u16 width = NULL, u16 height = NULL) override;

	void SetCameraPosition(CameraPosition3D pos) override;
	void ShowFrameStats(milliseconds& dt);
	
	// no copy, no move
	DX12Engine(DX12Engine& rhs) = delete;
	DX12Engine(const DX12Engine& rhs) = delete;
	DX12Engine& operator=(DX12Engine& rhs) = delete;
	DX12Engine& operator=(const DX12Engine& rhs) = delete;
};