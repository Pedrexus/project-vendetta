#pragma once

#include <pch.h>
#include <types.h>

////////////////////////////////////////////////////
//
// Chapter 9, page 242
//
// These are the public APIs for any object that implements reactions
// to events sent by hardware user interface devices.
//
////////////////////////////////////////////////////

class IKeyboardHandler
{
public:
	virtual bool OnKeyDown(const u8 c) = 0;
	virtual bool OnKeyUp(const u8 c) = 0;
};

class IPointerHandler
{
public:
	virtual bool OnPointerMove(const Point& pos, const int radius) = 0;
	virtual bool OnPointerButtonDown(const Point& pos, const int radius, const std::string& buttonName) = 0;
	virtual bool OnPointerButtonUp(const Point& pos, const int radius, const std::string& buttonName) = 0;
};

// unused
class IJoystickHandler
{
	virtual bool OnButtonDown(const std::string& buttonName, const int pressure) = 0;
	virtual bool OnButtonUp(const std::string& buttonName) = 0;
	virtual bool OnJoystick(const float x, const float y) = 0;
};

// unused
class IGamepadHandler
{
	virtual bool OnTrigger(const std::string& triggerName, const float pressure) = 0;
	virtual bool OnButtonDown(const std::string& buttonName, const int pressure) = 0;
	virtual bool OnButtonUp(const std::string& buttonName) = 0;
	virtual bool OnDirectionalPad(const std::string& direction) = 0;
	virtual bool OnThumbstick(const std::string& stickName, const float x, const float y) = 0;
};