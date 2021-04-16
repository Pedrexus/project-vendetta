#pragma once

#include "../../dx12pch.h"
#include "PipelineStageShader.h"

class HLSLShaders
{
	PipelineStageShader m_vs;
	PipelineStageShader m_ps;

public:
	HLSLShaders(std::wstring filename, D3D_SHADER_MACRO* defines) :
		m_vs(filename, defines, VertexShader),
		m_ps(filename, defines, PixelShader)
	{}

	HLSLShaders(std::wstring vs, std::wstring ps, D3D_SHADER_MACRO* defines) :
		m_vs(vs, defines, VertexShader),
		m_ps(ps, defines, PixelShader)
	{}

	inline D3D12_SHADER_BYTECODE GetVSByteCode() { return m_vs.GetByteCode(); }
	inline D3D12_SHADER_BYTECODE GetPSByteCode() { return m_ps.GetByteCode(); }
};