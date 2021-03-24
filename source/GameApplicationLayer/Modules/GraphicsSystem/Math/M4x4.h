#pragma once

#include <pch.h>
#include <types.h>

#include "VEC3.h"
#include "VEC4.h"

using namespace DirectX;

class M4x4 : public XMFLOAT4X4
{
public:
	// Constructors
	M4x4(XMFLOAT4X4& mat) { memcpy(&m, &mat.m, sizeof(mat.m)); }
	M4x4() : XMFLOAT4X4() {}

	// Modifiers
	inline void SetPosition(VEC3 const& pos);
	inline void SetPosition(VEC4 const& pos);
	inline void SetScale(VEC3 const& scale);

	// Accessors and Calculation Methods
	inline VEC3 GetPosition() const { return VEC3(m[3][0], m[3][1], m[3][2]); };
	inline VEC3 GetDirection() const;
	inline VEC3 GetUp() const;
	inline VEC3 GetRight() const;
	inline VEC3 GetYawPitchRoll() const;
	inline VEC3 GetScale() const;
	inline VEC4 Xform(VEC4& v) const;
	inline VEC3 Xform(VEC3& v) const;
	inline M4x4 Inverse() const;

	// static const M4x4 g_Identity;

	// Initialization methods
	inline void BuildTranslation(const VEC3& pos);
	inline void BuildTranslation(const float x, const float y, const float z);
	inline void BuildRotationX(const float radians) { XMMatrixRotationX(radians); }
	inline void BuildRotationY(const float radians) { XMMatrixRotationY(radians); }
	inline void BuildRotationZ(const float radians) { XMMatrixRotationZ(radians); }
	inline void BuildYawPitchRoll(const float yawRadians, const float pitchRadians, const float rollRadians)
	{
		D3DXMatrixRotationYawPitchRoll(this, yawRadians, pitchRadians, rollRadians);
	}
	inline void BuildRotationQuat(const Quaternion& q) { D3DXMatrixRotationQuaternion(this, &q); }
	inline void BuildRotationLookAt(const VEC3& eye, const VEC3& at, const VEC3& up) { D3DXMatrixLookAtRH(this, &eye, &at, &up); }
	inline void BuildScale(const float x, const float y, const float z);
};


inline void M4x4::SetPosition(VEC3 const& pos)
{
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
	m[3][3] = 1.0f;
}

inline void M4x4::SetPosition(VEC4 const& pos)
{
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
	m[3][3] = pos.w;
}

inline void M4x4::SetScale(VEC3 const& scale)
{
	m[1][1] = scale.x;
	m[2][2] = scale.y;
	m[3][3] = scale.z;
}

inline Vec3 Mat4x4::GetDirection() const
{
	// Note - the following code can be used to double check the vector construction above.
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f, 0.f, 0.f));
	Vec3 forward = justRot.Xform(g_Forward);
	return forward;
}

inline Vec3 Mat4x4::GetRight() const
{
	// Note - the following code can be used to double check the vector construction above.
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f, 0.f, 0.f));
	Vec3 right = justRot.Xform(g_Right);
	return right;
}

inline Vec3 Mat4x4::GetUp() const
{
	// Note - the following code can be used to double check the vector construction above.
	Mat4x4 justRot = *this;
	justRot.SetPosition(Vec3(0.f, 0.f, 0.f));
	Vec3 up = justRot.Xform(g_Up);
	return up;
}




inline Vec3 Mat4x4::GetYawPitchRoll() const
{
	float yaw, pitch, roll;

	pitch = asin(-_32);

	double threshold = 0.001; // Hardcoded constant - burn him, he's a witch
	double test = cos(pitch);

	if (test > threshold)
	{
		roll = atan2(_12, _22);
		yaw = atan2(_31, _33);
	}
	else
	{
		roll = atan2(-_21, _11);
		yaw = 0.0;
	}

	return (Vec3(yaw, pitch, roll));
}


inline Vec3 Mat4x4::GetScale() const
{
	return Vec3(m[0][0], m[1][1], m[2][2]);
}

inline Vec4 Mat4x4::Xform(Vec4& v) const
{
	Vec4 temp;
	D3DXVec4Transform(&temp, &v, this);
	return temp;
}

inline Vec3 Mat4x4::Xform(Vec3& v) const
{
	Vec4 temp(v);
	Vec4 out;
	D3DXVec4Transform(&out, &temp, this);
	return Vec3(out.x, out.y, out.z);
}

inline Mat4x4 Mat4x4::Inverse() const
{
	Mat4x4 out;
	D3DXMatrixInverse(&out, NULL, this);
	return out;
}

inline void Mat4x4::BuildTranslation(const Vec3& pos)
{
	*this = Mat4x4::g_Identity;
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
}

inline void Mat4x4::BuildTranslation(const float x, const float y, const float z)
{
	*this = Mat4x4::g_Identity;
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}

inline void Mat4x4::BuildScale(const float x, const float y, const float z)
{
	*this = Mat4x4::g_Identity;
	m[1][1] = x;
	m[2][2] = y;
	m[3][3] = z;
}


inline Mat4x4 operator * (const Mat4x4& a, const Mat4x4& b)
{
	Mat4x4 out;
	D3DXMatrixMultiply(&out, &a, &b);

	return out;
}