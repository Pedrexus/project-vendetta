#pragma once

#include <dx12pch.h>

// TODO: reorder in .hlsl for better packing
struct Light
{
	XMFLOAT3 Position;
	f32 FalloffStart;

	XMFLOAT3 Direction;
	f32 FalloffEnd;

	XMFLOAT3 Intensity;
	f32 SpotPower;
};