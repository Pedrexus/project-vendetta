#pragma once

#include <dx12pch.h>

#include "IBuffer.h"

/*
    A dynamic buffer uploads T* data to the GPU.
    After uploading, the data in the resource can be updated by the CPU.
*/
template<typename T>
class DynamicBuffer : public IBuffer
{
    u8* _MappedData = nullptr; // bytearray
    u64 _ByteSize = 0;

public:
    DynamicBuffer() = default;
    DynamicBuffer(const DynamicBuffer& rhs) = delete;
    DynamicBuffer& operator=(const DynamicBuffer& rhs) = delete;
    virtual ~DynamicBuffer()
    {
        if (_MainBuffer != nullptr)
            _MainBuffer->Unmap(0, nullptr);
        _MappedData = nullptr;
    }

    inline virtual u64 GetElementByteSize() const { return sizeof(T); };

    inline void Create(ID3D12Device* device, u64 elementCount)
    {
        _ByteSize = GetElementByteSize() * elementCount;

        auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto DynamicBuffer = CD3DX12_RESOURCE_DESC::Buffer(_ByteSize);

        ThrowIfFailed(device->CreateCommittedResource(
            &uploadHeap,
            D3D12_HEAP_FLAG_NONE,
            &DynamicBuffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&_MainBuffer))
        );

        ThrowIfFailed(_MainBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_MappedData)));
    }

    // uploads array of elements
    inline void Upload(const T* data) 
    {
        CopyMemory(&_MappedData[0], data, _ByteSize);
    }

    // uploads one element
    inline void Upload(i32 elementIndex, const T& data)
    {
        // we copy the data to the pointer ->Map() has given us
        CopyMemory(&_MappedData[elementIndex * GetElementByteSize()], &data, GetElementByteSize());
    }

    struct BUFFER_VIEW
    {
        D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;
        u32 TotalByteSize;
        u32 ElementByteSize;
    };

    BUFFER_VIEW GetBufferView() const
    {
        return { IBuffer::operator->()->GetGPUVirtualAddress(), (u32) _ByteSize, (u32) GetElementByteSize() };
    }

    // TODO: upload region (startIndex, size, data)
};