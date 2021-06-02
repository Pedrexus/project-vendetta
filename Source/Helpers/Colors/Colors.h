#pragma once

#include <types.h>
#include <DirectXMath.h>

DirectX::XMVECTORF32 FromHex(u32 hexValue)
{
	auto r = ((hexValue >> 16) & 0xFF) / 255.0f;
	auto g = ((hexValue >> 8) & 0xFF) / 255.0f;
	auto b = ((hexValue) & 0xFF) / 255.0f;
	return { r, g, b, 1.0f };
}