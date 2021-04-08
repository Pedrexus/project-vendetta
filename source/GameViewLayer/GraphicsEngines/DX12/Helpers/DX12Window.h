#pragma once

#include "../dx12pch.h"

D3D12_VIEWPORT CreateViewport(u32 width, u32 height)
{
	// Update the viewport transform to cover the client area.
	static D3D12_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<f32>(width);
	vp.Height = static_cast<f32>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	return vp;
}

D3D12_RECT CreateScissorRectangle(u32 width, u32 height)
{
	return { 0, 0, (i32) width, (i32) height };
}