#pragma once

#include <types.h>

#include "../Interfaces/InputHandlers.h"

class IGraphicsEngine
{
public:
	virtual ~IGraphicsEngine() = default;

	virtual void Initialize() = 0;
	virtual bool IsReady() = 0;

	virtual void SetCameraPosition(CameraPosition3D pos) = 0;
	
	virtual void OnUpdate(milliseconds dt) = 0;
	virtual void OnDraw() = 0;
	virtual void OnResize(u32 width = NULL, u32 height = NULL) = 0;
};