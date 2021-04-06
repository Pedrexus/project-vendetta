#pragma once

#include "../dx12pch.h"

namespace Shaders
{

	/*
		Compiles shader at runtime

		In order to compile the shader offline, we use FXC

		fxc “color.hlsl” /Od /Zi /T vs_5_0 /E “VS” /Fo “color_vs.cso” /Fc “color_vs.asm”
	*/
	ComPtr<ID3DBlob> Compile(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);

	ComPtr<ID3DBlob> ReadCompiled(const std::wstring& filename, bool useResourceCache = false);

	D3D12_SHADER_BYTECODE LoadCompiled(const std::wstring& filename, bool useResourceCache = false);
}