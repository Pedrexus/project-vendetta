#pragma once

#include "../../dx12pch.h"

class IBuffer
{
protected:
	ComPtr<ID3D12Resource> _MainBuffer;
	u64 _ByteSize;

public:
	virtual void Create(ID3D12Device* device, u64 byteSize) = 0;
	virtual void Upload(ID3D12GraphicsCommandList* cmdList, const void* data) = 0;

	inline ID3D12Resource* operator->()
	{
		return _MainBuffer.Get();
	}

	inline void Dispose()
	{
		_MainBuffer.Reset();
	}
};