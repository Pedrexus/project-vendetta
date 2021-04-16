#pragma once

#include "../../dx12pch.h"
#include "../Buffers/UploadBuffer.h"
#include "../Buffers/ConstantBuffer.h"

struct CameraConstants
{
	XMMATRIX ViewProj;
};

class Camera
{
	inline static const auto target = XMVectorZero();
	inline static const auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	inline static const auto fovAngleY = XM_PIDIV4;
	inline static const auto nearZ = 1.0f;
	inline static const auto farZ = 1000.0f;
	
	XMMATRIX _View = XMMatrixIdentity();
	XMMATRIX _Proj = XMMatrixIdentity();

	ConstantBuffer<CameraConstants> constantBuffer;

public:
	Camera(ID3D12Device* device) : constantBuffer(device, 1) {};

	inline void UpdateCameraView(CameraPosition3D pos)
	{
		auto position = XMVectorSet(pos[0], pos[1], pos[2], 1.0f);
		UpdateView(position);

		// Update the constant buffer with the latest worldViewProj matrix.
		constantBuffer.CopyToCPUBuffer(0, { GetViewProj() });
	};

	inline void UpdateView(XMVECTOR pos)
	{
		_View = XMMatrixLookAtLH(pos, target, up);
	}

	inline XMMATRIX GetViewProj()
	{
		return _View * _Proj;
	}

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	inline void Resize(u32 width, u32 height)
	{
		auto aspectRatio = static_cast<f32>(width) / height;
		_Proj = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
	}

	inline ID3D12DescriptorHeap* GetBufferHeap()
	{
		return constantBuffer.cbvHeap.heap.Get();
	}

	inline D3D12_GPU_DESCRIPTOR_HANDLE GetBufferGPUHandle()
	{
		return constantBuffer.cbvHeap.GetGPUHandle();
	}

};