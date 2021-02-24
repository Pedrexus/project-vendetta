#pragma once

#include <pch.h>

// TODO: Mat4x4 -> DirectX::XMMATRIX

//
// class IRenderer
//
// class IRenderer abstracts the renderer implementation so the engine
//   can use D3D9 or D3D11.
//
class IRenderer
{
public:
	virtual void SetBackgroundColor(BYTE bgA, BYTE bgR, BYTE bgG, BYTE bgB) = 0;
	virtual HRESULT VOnRestore() = 0;
	virtual void Shutdown() = 0;
	virtual bool PreRender() = 0;
	virtual bool PostRender() = 0;
	// virtual void CalcLighting(Lights* lights, int maximumLights) = 0;
	// virtual void SetWorldTransform(const Mat4x4* m) = 0;
	// virtual void SetViewTransform(const Mat4x4* m) = 0;
	// virtual void SetProjectionTransform(const Mat4x4* m) = 0;
	// virtual std::shared_ptr<IRenderState> PrepareAlphaPass() = 0;
	// virtual std::shared_ptr<IRenderState> PrepareSkyBoxPass() = 0;
	// virtual void DrawLine(const Vec3& from, const Vec3& to, const Color& color) = 0;
};