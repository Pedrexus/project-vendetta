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


std::string PipelineStageShader::GetStageTarget()
{
	switch (m_PipelineStage)
	{
		case VertexShader: return "vs_5_0";
		case PixelShader: return "ps_5_0";
		case HullShader: return "hs_5_0";
		case GeometryShader: return "gs_5_0";
		case DomainShader: return "ds_5_0";
		case ComputeShader: return "cs_5_0";
		default:
			throw std::exception("Invalid Shader Stage");
	}
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
			throw std::exception("Invalid Shader Stage");
	}
}

D3D12_SHADER_BYTECODE PipelineStageShader::GetByteCode()
{
	auto pBuffer = reinterpret_cast<BYTE*>(m_ByteCode->GetBufferPointer());
	auto bufferSize = m_ByteCode->GetBufferSize();
	return { pBuffer, bufferSize };
}