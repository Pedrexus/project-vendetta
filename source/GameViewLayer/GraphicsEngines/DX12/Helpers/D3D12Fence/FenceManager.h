#pragma once

#include "../../dx12pch.h"

/*
	A fence is identified by an integer.
	At each fence point, we increment it.
*/
class FenceManager
{
	ComPtr<ID3D12Fence> fence;
	u64 currentFence = 0;

protected:
	static inline HANDLE CreateCPUEvent()
	{
		HANDLE eventHandle = CreateEventEx(nullptr, L"Flush Command Queue Event", false, EVENT_ALL_ACCESS);
		
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			LOG_ERROR("Flush Command Queue Event Already exists. GPU may be left on a broken state.");
		else if(!eventHandle)
			LOG_ERROR("Event Handle is null");
		
		return eventHandle;
	}

public:
	FenceManager(ID3D12Device* device);

	inline ID3D12Fence* Get() { return fence.Get(); }

	// Advance the fence value to mark commands up to this fence point.
	inline u64 Advance() { return ++currentFence; };

	inline bool IsSynchronized() { return fence->GetCompletedValue() >= currentFence; }

	inline void WaitForGPU()
	{
		auto CPUEventHandle = CreateCPUEvent();
		ThrowIfFailed(fence->SetEventOnCompletion(currentFence, CPUEventHandle));
		WaitForSingleObject(CPUEventHandle, INFINITE);
		CloseHandle(CPUEventHandle);
	}


};