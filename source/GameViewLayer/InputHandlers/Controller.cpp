#include "Controller.h"

#include <DirectXMath.h>

static f32 degreesPerPixel = .25f;
static f32 lengthPerPixel = 0.05f;

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

void Controller::OnPointerMove(const Point& pos, const int radius)
{
	auto dx = pos.x - m_lastMousePos.x;
	auto dy = pos.y - m_lastMousePos.y;

	if (IsLeftPointerDown())
	{
		m_Theta += CalcDeltaAngleInRadians(dx);
		m_Phi += CalcDeltaAngleInRadians(dy);
		m_Phi = Threshold<f32>(m_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}
	else if (IsRightPointerDown())
	{
		m_Radius += CalcDeltaZoomInLengthUnits(dx, dy);
		m_Radius = Threshold<f32>(m_Radius, 3.0f, 15.0f);
	}

	m_lastMousePos = pos;
}

void Controller::OnPointerButtonDown(const Point& pos, const int radius, const i32 button)
{
	m_lastMousePos = pos;
	m_btnState = button;
}

void Controller::OnPointerButtonUp(const Point& pos, const int radius, const i32 button)
{
	m_btnState = NULL;
}

CameraPosition3D Controller::GetCameraPosition()
{
	auto x = m_Radius * sin(m_Phi) * cos(m_Theta);
	auto z = m_Radius * sin(m_Phi) * sin(m_Theta);
	auto y = m_Radius * cos(m_Phi);

	return { x, y, z };
}