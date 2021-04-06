#pragma once

#include <pch.h>
#include <types.h>

using namespace DirectX;

class VEC3 : public XMFLOAT3
{
protected:
	inline XMVECTOR AsXMVector() const { return XMLoadFloat3(this); }
public:
	inline f32 Length() { return XMVectorGetX(XMVector3Length(AsXMVector())); }
	inline VEC3* Normalize() { XMStoreFloat3(this, XMVector3Normalize(AsXMVector())); return this; }
	inline f32 Dot(const VEC3& b) { return XMVectorGetX(XMVector3Dot(AsXMVector(), b.AsXMVector())); }
	inline VEC3 Cross(const VEC3& b) const { return VEC3(XMVector3Cross(AsXMVector(), XMLoadFloat3(&b))); }

	// constructors
	VEC3(XMVECTOR v3) { x = XMVectorGetX(v3); y = XMVectorGetY(v3); z = XMVectorGetZ(v3); }
	VEC3(XMFLOAT3& v3) { x = v3.x; y = v3.y; z = v3.z; }
	VEC3() : XMFLOAT3() { x = 0; y = 0; z = 0; }
	VEC3(const f32 _x, const f32 _y, const f32 _z) { x = _x; y = _y; z = _z; }
	VEC3(const f64 _x, const f64 _y, const f64 _z) { x = (f32) _x; y = (f32) _y; z = (f32) _z; }
	inline VEC3(const VEC4& v4) { x = v4.x; y = v4.y; z = v4.z; }
};