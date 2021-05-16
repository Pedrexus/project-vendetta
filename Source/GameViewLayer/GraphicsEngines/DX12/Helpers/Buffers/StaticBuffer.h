#pragma once

#include "IBuffer.h"

/*
    A static buffer uploads void* data to the GPU.
    After uploading, the resources cannot be changed.
*/
class StaticBuffer : public IBuffer
{
    ComPtr<ID3D12Resource> _IntermediateBuffer;

    CD3DX12_RESOURCE_BARRIER GetCopyTransition();
    CD3DX12_RESOURCE_BARRIER GetReadTransition();

public:
    StaticBuffer() = default;
    StaticBuffer(StaticBuffer& rhs) = delete;
    StaticBuffer& operator=(const StaticBuffer& rhs) = delete;

    void Create(ID3D12Device* device, u64 byteSize);
    void Upload(ID3D12GraphicsCommandList* cmdList, const void* data);
    void Dispose();
};