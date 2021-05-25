#pragma once

#include <dx12pch.h>

class IBuffer
{
protected:
	ComPtr<ID3D12Resource> _MainBuffer;
	u64 _ByteSize;

public:
	inline ID3D12Resource* operator->() const
	{
		return _MainBuffer.Get();
	}

	inline void Dispose()
	{
		_MainBuffer.Reset();
	}
};