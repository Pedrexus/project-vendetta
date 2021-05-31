#pragma once

// Link necessary d3d12 libraries.
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib,"D3DCompiler.lib")

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <pch.h>
#include <macros.h>
#include <Helpers/Functions.h>

#include <Dependencies/DirectX12/d3dx12.h>
#include <GameViewLayer/GraphicsEngines/DX12/Common/ThrowIfFailed.h>

#include <pix3.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <D3d12SDKLayers.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>

// DirectX Tool Kit 12
#include <DirectXTK12/SimpleMath.h>
#include <DirectXTK12/CommonStates.h>
#include <DirectXTK12/BufferHelpers.h>
#include <DirectXTK12/DirectXHelpers.h>
#include <DirectXTK12/DescriptorHeap.h>
#include <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/GeometricPrimitive.h>
#include <DirectXTK12/ResourceUploadBatch.h>

#include <DirectXTK12/Effects.h>
#include <DirectXTK12/EffectPipelineStateDescription.h>

using namespace DX;
using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;
