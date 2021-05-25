#pragma once

#include <dx12pch.h>

// Direct3D will enable extra debugging and send debug messages to the VC++ output window
inline void EnableDebugLayer()
{
		ComPtr<ID3D12Debug3> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
}