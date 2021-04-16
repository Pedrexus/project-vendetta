#pragma once

#include "../../dx12pch.h"

class Camera
{
	inline static const auto target = XMVectorZero();
	inline static const auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	inline static const auto fovAngleY = XM_PIDIV4;
	inline static const auto nearZ = 1.0f;
	inline static const auto farZ = 1000.0f;
	
	XMMATRIX _View = XMMatrixIdentity();
	XMMATRIX _Proj = XMMatrixIdentity();

public:
	Camera() = default;

	inline void UpdateCameraView(CameraPosition3D pos)
	{
		auto position = XMVectorSet(pos[0], pos[1], pos[2], 1.0f);
		UpdateView(position);
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

};