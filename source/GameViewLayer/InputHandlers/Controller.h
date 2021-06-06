#pragma once

#include <DirectXMath.h>

#include "../Interfaces/InputHandlers.h"


class Controller : public IPointerHandler // TODO: public IKeyboardHandler
{
	Point m_lastMousePos;
	i32 m_btnState;

	f32 m_Theta = 0;
	f32 m_Phi = 1;
	f32 m_Radius = 10;

protected:
	bool IsLeftPointerDown() { return m_btnState == MK_LBUTTON; }
	bool IsRightPointerDown() { return m_btnState == MK_RBUTTON; }

public:
	void OnPointerMove(const Point& pos, const int radius) override;
	void OnPointerButtonDown(const Point& pos, const int radius, const i32 button) override;
	void OnPointerButtonUp(const Point& pos, const int radius, const i32 button) override;
	CameraPosition3D GetCameraPosition() override;
};