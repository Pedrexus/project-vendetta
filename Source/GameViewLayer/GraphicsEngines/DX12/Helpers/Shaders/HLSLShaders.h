#pragma once

#include <dx12pch.h>

#include "PipelineStageShader.h"

class HLSLShaders
{
	PipelineStageShader m_vs;
	PipelineStageShader m_ps;

public:
	HLSLShaders(std::wstring filename, const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel, D3D_SHADER_MACRO* defines) :
		m_vs(filename, shaderModel, defines, VertexShader),
		m_ps(filename, shaderModel, defines, PixelShader)
	{}

	HLSLShaders(std::wstring vs, std::wstring ps, const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel, D3D_SHADER_MACRO* defines) :
		m_vs(vs, shaderModel, defines, VertexShader),
		m_ps(ps, shaderModel, defines, PixelShader)
	{}

	inline D3D12_SHADER_BYTECODE GetVSByteCode() { return m_vs.GetByteCode(); }
	inline D3D12_SHADER_BYTECODE GetPSByteCode() { return m_ps.GetByteCode(); }
};