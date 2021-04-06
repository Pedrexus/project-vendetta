#include "ShaderCompiler.h"

ComPtr<ID3DBlob> Shaders::Compile(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
#ifdef _DEBUG
	auto compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
	auto compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3; // highest optimization level
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;

	hr = D3DCompileFromFile(
		filename.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(),
		target.c_str(),
		compileFlags,
		NULL,
		&byteCode,
		&errors
	);

	if (errors != nullptr)
		LOG_ERROR((char*) errors->GetBufferPointer());
	ThrowIfFailed(hr);

	return byteCode; // the compiled bytecode is returned
}

ComPtr<ID3DBlob> Shaders::ReadCompiled(const std::wstring& filename, bool useResourceCache)
{
	if (useResourceCache)
		throw std::exception("resource cache support not implemented");

	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (i32) fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	ComPtr<ID3DBlob> blob;
	ThrowIfFailed(D3DCreateBlob(size, blob.GetAddressOf()));

	fin.read((char*) blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}

D3D12_SHADER_BYTECODE Shaders::LoadCompiled(const std::wstring& filename, bool useResourceCache)
{
	auto bytecode = ReadCompiled(filename, useResourceCache);

	D3D12_SHADER_BYTECODE sb = {};
	sb.pShaderBytecode = reinterpret_cast<BYTE*>(bytecode->GetBufferPointer());
	sb.BytecodeLength = bytecode->GetBufferSize();

	return sb;
}
