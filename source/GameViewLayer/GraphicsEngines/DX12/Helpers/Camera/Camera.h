#pragma once

#include <dx12pch.h>
#include <GameViewLayer/Interfaces/InputHandlers.h>

class Camera
{
	inline static const auto target = Vector3::Zero;
	inline static const auto up = Vector3::UnitY;
	inline static const auto fovAngleY = XM_PIDIV4;
	inline static const auto nearZ = 1.0f;
	inline static const auto farZ = 1000.0f;
	
	Vector3 _eye = Vector3::Zero;
	Matrix _view = Matrix::Identity;
	Matrix _proj = Matrix::Identity;

public:
	Camera() = default;

	inline void UpdateCameraView(CameraPosition3D pos)
	{
		_eye = std::move(Vector3(pos.data()));
		_view = Matrix::CreateLookAt(_eye, target, up);
	};

	inline XMMATRIX GetViewProj()
	{
		return _view * _proj;
	}

	inline XMFLOAT3& GetEyePosition()
	{
		return _eye;
	}

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	inline void Resize(u16 width, u16 height)
	{
		auto aspectRatio = static_cast<f32>(width) / height;
		_proj = Matrix::CreatePerspectiveFieldOfView(fovAngleY, aspectRatio, nearZ, farZ);
	}

};