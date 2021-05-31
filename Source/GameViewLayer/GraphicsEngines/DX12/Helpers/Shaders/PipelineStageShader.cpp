#include "PipelineStageShader.h"

ComPtr<ID3DBlob> PipelineStageShader::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	u32 compileFlags = 0;
#ifdef _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode;
	ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		LOG_ERROR((char*) errors->GetBufferPointer());

	ThrowIfFailed(hr);

	return byteCode;
}


std::string PipelineStageShader::GetStageTarget(const D3D12_FEATURE_DATA_SHADER_MODEL& shaderModel)
{
	auto model = [&] ()
	{
		LOG_WARNING("Unable to compile shader model 6 with D3DCompileFromFile");
		return "5_1";

		switch (shaderModel.HighestShaderModel)
		{
			case D3D_SHADER_MODEL_5_1: return "5_1";
			case D3D_SHADER_MODEL_6_0: return "6_0";
			case D3D_SHADER_MODEL_6_1: return "6_1";
			case D3D_SHADER_MODEL_6_2: return "6_2";
			case D3D_SHADER_MODEL_6_3: return "6_3";
			case D3D_SHADER_MODEL_6_4: return "6_4";
			case D3D_SHADER_MODEL_6_5: return "6_5";
			case D3D_SHADER_MODEL_6_6: return "6_6";
			default:
				return "";
		};
	};

	auto stage = [&] ()
	{
		switch (m_PipelineStage)
		{
			case VertexShader:		return "vs";
			case PixelShader:		return "ps";
			case HullShader:		return "hs";
			case GeometryShader:	return "gs";
			case DomainShader:		return "ds";
			case ComputeShader:		return "cs";
			default:
				throw std::exception("Invalid Shader Stage");
		};
	};

	return std::format("{}_{}", stage(), model());
}

std::string PipelineStageShader::GetStageEntrypoint()
{
	switch (m_PipelineStage)
	{
		case VertexShader: return "VS";
		case PixelShader: return "PS";
		case HullShader: return "HS";
		case GeometryShader: return "GS";
		case DomainShader: return "DS";
		case ComputeShader: return "CS";
		default:
			throw std::exception("Invalid Shader Entrypoint");
	}
}

D3D12_SHADER_BYTECODE PipelineStageShader::GetByteCode()
{
	auto pBuffer = reinterpret_cast<BYTE*>(m_ByteCode->GetBufferPointer());
	auto bufferSize = m_ByteCode->GetBufferSize();
	return { pBuffer, bufferSize };
}