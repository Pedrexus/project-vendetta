#pragma once

#include "../../dx12pch.h"

class DefaultBuffer
{
    ComPtr<ID3D12Resource> _DefaultBuffer;
    ComPtr<ID3D12Resource> _IntermediateBuffer;

    CD3DX12_RESOURCE_BARRIER GetCopyTransition()
    {
        return CD3DX12_RESOURCE_BARRIER::Transition(_DefaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    }

    CD3DX12_RESOURCE_BARRIER GetReadTransition()
    {
        return CD3DX12_RESOURCE_BARRIER::Transition(_DefaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
    }

public:
    DefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, u64 byteSize);
    void Dispose();

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() { return _DefaultBuffer->GetGPUVirtualAddress(); }
};