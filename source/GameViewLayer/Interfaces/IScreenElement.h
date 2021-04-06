#pragma once

#include <pch.h>
#include <types.h>

#include "Enums.h"

class IScreenElement
{
public:
	virtual HRESULT OnRestore() = 0;
	virtual HRESULT OnLostDevice() = 0;
	virtual HRESULT OnRender(f64 time, f32 elapsedTime) = 0;
	virtual void OnUpdate(u32 dt) = 0;

	virtual int GetZOrder() const = 0;
	virtual void SetZOrder(const i32 zOrder) = 0;
	virtual bool IsVisible() const = 0;
	virtual void SetVisible(bool visible) = 0;

	virtual LRESULT CALLBACK OnMsgProc(MSG msg) = 0;

	virtual ~IScreenElement() {};
	virtual bool const operator <(IScreenElement const& other) { return GetZOrder() < other.GetZOrder(); }
};