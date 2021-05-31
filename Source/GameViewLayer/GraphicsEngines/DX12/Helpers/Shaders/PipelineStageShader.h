#pragma once

#include <dx12pch.h>

enum Stage
{
	VertexShader,
	PixelShader,
	HullShader,
	GeometryShader,
	DomainShader,
	ComputeShader
};

class PipelineStageShader
{
	ComPtr<ID3DBlob> m_ByteCode = nullptr;
	Stage m_PipelineStage;

	static ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target
	);

	std::string GetStageTarget(const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel);
	std::string GetStageEntrypoint();

public:
	PipelineStageShader(const std::wstring& filename, const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel, const D3D_SHADER_MACRO* defines, Stage stage) :
		m_PipelineStage(stage)
	{
		auto entrypoint = GetStageEntrypoint();
		auto target = GetStageTarget(shaderModel);
		m_ByteCode = CompileShader(filename, defines, entrypoint, target);
	}

	D3D12_SHADER_BYTECODE GetByteCode();
};