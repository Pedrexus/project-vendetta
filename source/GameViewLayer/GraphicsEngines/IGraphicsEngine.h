#pragma once

#include <types.h>

#include "../Interfaces/InputHandlers.h"

class IGraphicsEngine
{
public:
	virtual ~IGraphicsEngine() = default;

	virtual void Initialize(HWND window, u16 width, u16 height) = 0;
	virtual bool IsReady() = 0;

	virtual void SetCameraPosition(CameraPosition3D pos) = 0;
	
	virtual void OnUpdate(milliseconds dt) = 0;
	virtual void OnDraw() = 0;
	virtual void OnResize(u16 width = NULL, u16 height = NULL) = 0;
};