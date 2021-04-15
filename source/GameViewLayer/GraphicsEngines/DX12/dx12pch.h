#pragma once

// Link necessary d3d12 libraries.
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib,"D3DCompiler.lib")

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <macros.h>
#include <Helpers/Functions.h>
#include <Dependencies/DirectX12/d3dx12.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <D3d12SDKLayers.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#define ThrowIfFailed(hr) if(FAILED(hr)) { LOG_FATAL("DirectX has failed with HRESULT " + Convert::int2hex(hr)); }