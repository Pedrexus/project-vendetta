#pragma once

#include <dx12pch.h>
#include <GameViewLayer/Interfaces/InputHandlers.h>

#include <DirectXTK12/Mouse.h>

static const auto degreesPerPixel = .25f;
static const auto lengthPerPixel = 0.05f;

inline f32 CalcDeltaAngleInRadians(i32 numberOfPixels)
{
	auto dthetaInDegrees = numberOfPixels * degreesPerPixel;
	return DirectX::XMConvertToRadians(dthetaInDegrees);
}

inline f32 CalcDeltaZoomInLengthUnits(i32 dx, i32 dy)
{
	auto horizontalZoom = lengthPerPixel * dx;
	auto verticalZoom = lengthPerPixel * dy;
	return horizontalZoom - verticalZoom;
}

template<typename T>
static T Threshold(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x);
}

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

	DirectX::Mouse _mouse;
	DirectX::Mouse::ButtonStateTracker _mouseButtons;

protected:
	inline void UpdateCameraView(Vector3 pos)
	{
		_eye = std::move(pos);
		_view = Matrix::CreateLookAt(_eye, target, up);
	};

public:
	Camera() = default;

	inline void Update()
	{
		static f32 theta = 0;
		static f32 phi = 1;
		static f32 radius = 10;

		if (_mouse.IsConnected())
		{
			auto last = _mouseButtons.GetLastState();
			auto curr = _mouse.GetState();

			auto dx = curr.x - last.x;
			auto dy = curr.y - last.y;

			// camera move
			{
				theta += CalcDeltaAngleInRadians(dx);
				phi += CalcDeltaAngleInRadians(dy);
				phi = Threshold<f32>(phi, 0.1f, DirectX::XM_PI - 0.1f);
				
				auto x = radius * sin(phi) * cos(theta);
				auto z = radius * sin(phi) * sin(theta);
				auto y = radius * cos(phi);

				UpdateCameraView({ x, y, z });
			}

			_mouseButtons.Update(curr);
		}	
	}

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