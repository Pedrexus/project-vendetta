#pragma once

#include "../../dx12pch.h"

/*
	A fence is identified by an integer.
	At each fence point, we increment it.
*/
class FenceManager
{
	ComPtr<ID3D12Fence> _Fence;
	u64 _CurrentFence = 0;
	u64 _Synchronizations = 0;

protected:
	static inline HANDLE CreateCPUEvent(u64 fence)
	{
		auto name = fmt::format(L"Flush Command Queue Event ", fence);
		HANDLE eventHandle = CreateEventEx(nullptr, name.c_str(), false, EVENT_ALL_ACCESS);

		if (GetLastError() == ERROR_ALREADY_EXISTS)
			LOG_WARNING("Flush Command Queue Event Already exists. GPU may be left on a broken state.");
		if (GetLastError() == ERROR_INVALID_HANDLE)
			LOG_WARNING("Event name is already in use. GPU may be left on a broken state.");
		else if (!eventHandle)
			LOG_ERROR("Event Handle is null");

		return eventHandle;
	}

public:
	FenceManager(ID3D12Device* device);
	~FenceManager()
	{
		LOG_INFO(fmt::format("Synced with GPU {} times.", _Synchronizations));
	}

	inline ID3D12Fence* Get() { return _Fence.Get(); }

	// Advance the fence value to mark commands up to this fence point.
	inline u64 Advance() { return ++_CurrentFence; };

	inline bool IsSynchronized(u64 fence) { return _Fence->GetCompletedValue() >= fence; }
	inline bool IsSynchronized() { return IsSynchronized(_CurrentFence); }

	inline void WaitForGPU(u64 fence)
	{
		auto CPUEventHandle = CreateCPUEvent(fence);
		ThrowIfFailed(_Fence->SetEventOnCompletion(fence, CPUEventHandle));
		WaitForSingleObject(CPUEventHandle, INFINITE);
		CloseHandle(CPUEventHandle);

		_Synchronizations++;
	}
	inline void WaitForGPU() { WaitForGPU(_CurrentFence); }


};