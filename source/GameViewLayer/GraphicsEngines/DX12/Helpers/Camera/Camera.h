#pragma once

#include "../../dx12pch.h"

struct CameraConstants
{
	XMMATRIX WorldViewProj;
};

typedef std::unique_ptr<UploadBuffer<CameraConstants>> CameraConstantsUploadBuffer;

class CameraConstantBuffer
{
	friend class Camera;

protected:
	Descriptor::Heap cbvHeap;
	CameraConstantsUploadBuffer uploadBuffer = nullptr;

public:
	CameraConstantBuffer(ID3D12Device* device)
	{
		cbvHeap = Descriptor::Heap(device, Descriptor::ConstantBuffer::Type);
		
		uploadBuffer = std::make_unique<UploadBuffer<CameraConstants>>(device, 1, true);
		UINT objCBByteSize = CalcConstantBufferByteSize(sizeof(CameraConstants));

		auto cbAddress = uploadBuffer->Resource()->GetGPUVirtualAddress();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(CameraConstants));

		device->CreateConstantBufferView(&cbvDesc, cbvHeap.GetCPUHandle());
	}

	// Update the constant buffer with the latest worldViewProj matrix.
	inline void Upload(XMMATRIX worldViewProj)
	{
		uploadBuffer->CopyData(0, { worldViewProj });
	}
};

class Camera
{
	inline static const auto target = XMVectorZero();
	inline static const auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	inline static const auto fovAngleY = XM_PIDIV4;
	inline static const auto nearZ = 1.0f;
	inline static const auto farZ = 1000.0f;
	
	XMVECTOR position;
	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX proj = XMMatrixIdentity();

	CameraConstantBuffer constantBuffer;

public:
	Camera(ID3D12Device* device) : constantBuffer(device) {};

	inline void SetCameraPosition(CameraPosition3D pos)
	{
		position = XMVectorSet(pos[0], pos[1], pos[2], 1.0f);
		view = XMMatrixLookAtLH(position, target, up);
	};

	inline XMMATRIX GetWorldViewProj()
	{
		return world * view * proj;
	}

	inline void Update()
	{
		constantBuffer.Upload(GetWorldViewProj());
	}

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	inline void Resize(u32 width, u32 height)
	{
		auto aspectRatio = static_cast<f32>(width) / height;
		proj = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
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