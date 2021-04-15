#pragma once

#include "../../dx12pch.h"

template<typename T>
class UploadBuffer
{
    ComPtr<ID3D12Resource> _UploadBuffer = nullptr;
    u8* _MappedData = nullptr; // bytearray
    u64 _ElementByteSize = 0;

public:
    inline UploadBuffer(ID3D12Device* device, u64 elementCount)
    {
        _ElementByteSize = CalcBufferByteSize();

        auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto uploadBuffer = CD3DX12_RESOURCE_DESC::Buffer(_ElementByteSize * elementCount);

        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeap,
            D3D12_HEAP_FLAG_NONE,
            &uploadBuffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&_UploadBuffer))
        );

        ThrowIfFailed(_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_MappedData)));

        // We do not need to unmap until we are done with the resource.  However, we must not write to
        // the resource while it is in use by the GPU (so we must use synchronization techniques).
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    virtual ~UploadBuffer()
    {
        if (_UploadBuffer != nullptr)
            _UploadBuffer->Unmap(0, nullptr);
        _MappedData = nullptr;
    }

    inline virtual u64 CalcBufferByteSize() const { return sizeof(T); }

    inline ID3D12Resource* GetResource() const
    {
        return _UploadBuffer.Get();
    }

    inline virtual void CopyToCPUBuffer(i32 elementIndex, const T& data)
    {
        // we copy the data to the pointer ->Map() has given us
        memcpy(&_MappedData[elementIndex * _ElementByteSize], &data, _ElementByteSize);
    }
};