#pragma once

#include <pch.h>
#include <types.h>
#include <Helpers/Geometry/Point.h>

typedef std::array<f32, 3> CameraPosition3D;

class IPointerHandler
{
public:
	virtual void OnPointerMove(const Point& pos, const i32 radius) = 0;
	virtual void OnPointerButtonDown(const Point& pos, const i32 radius, const i32 button) = 0;
	virtual void OnPointerButtonUp(const Point& pos, const i32 radius, const i32 button) = 0;
	virtual CameraPosition3D GetCameraPosition() = 0;
};


class IKeyboardHandler
{
public:
	virtual bool OnKeyDown(const u8 c) = 0;
	virtual bool OnKeyUp(const u8 c) = 0;
};

// unused
class IJoystickHandler
{
	virtual bool OnButtonDown(const std::string& buttonName, const i32 pressure) = 0;
	virtual bool OnButtonUp(const std::string& buttonName) = 0;
	virtual bool OnJoystick(const f32 x, const f32 y) = 0;
};

// unused
class IGamepadHandler
{
	virtual bool OnTrigger(const std::string& triggerName, const f32 pressure) = 0;
	virtual bool OnButtonDown(const std::string& buttonName, const i32 pressure) = 0;
	virtual bool OnButtonUp(const std::string& buttonName) = 0;
	virtual bool OnDirectionalPad(const std::string& direction) = 0;
	virtual bool OnThumbstick(const std::string& stickName, const f32 x, const f32 y) = 0;
};