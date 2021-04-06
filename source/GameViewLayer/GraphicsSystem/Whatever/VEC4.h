#pragma once

#include <pch.h>
#include <types.h>

using namespace DirectX;

class VEC4 : public XMFLOAT4
{
protected:
	inline XMVECTOR AsXMVector() const { return XMLoadFloat4(this); }
public:
	inline f32 Length() { return XMVectorGetX(XMVector4Length(AsXMVector())); }
	inline VEC4* Normalize() { XMStoreFloat4(this, XMVector4Normalize(AsXMVector())); return this; }
	inline f32 Dot(const VEC4& b) { return XMVectorGetX(XMVector4Dot(AsXMVector(), b.AsXMVector())); }

	// constructors
	VEC4(XMVECTOR v4) { x = XMVectorGetX(v4); y = XMVectorGetY(v4); z = XMVectorGetZ(v4); w = XMVectorGetW(v4); }
	VEC4(XMFLOAT4& v4) { x = v4.x; y = v4.y; z = v4.z; w = v4.w; }
	VEC4() : XMFLOAT4() {}
	VEC4(const f32 _x, const f32 _y, const f32 _z, const f32 _w) { x = _x; y = _y; z = _z; w = _w; }
	VEC4(const f64 _x, const f64 _y, const f64 _z, const f64 _w) { x = (f32) _x; y = (f32) _y; z = (f32) _z; w = (f32) _w; }
	inline VEC4(const VEC3& v3) { x = v3.x; y = v3.y; z = v3.z; w = 1.0f; }
};