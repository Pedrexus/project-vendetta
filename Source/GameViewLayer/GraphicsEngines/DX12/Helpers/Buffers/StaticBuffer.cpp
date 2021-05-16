#include "StaticBuffer.h"


ComPtr<ID3D12Resource> CreateStaticBuffer(ID3D12Device* device, u64 byteSize)
{
    ComPtr<ID3D12Resource> StaticBuffer;

    auto bufferType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    // Create the actual default buffer resource.
    ThrowIfFailed(device->CreateCommittedResource(
        &bufferType,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(StaticBuffer.GetAddressOf())));

    return StaticBuffer;
}

ComPtr<ID3D12Resource> CreateIntermediateBuffer(ID3D12Device* device, u64 byteSize)
{
    ComPtr<ID3D12Resource> intermediateBuffer;

    auto bufferType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    ThrowIfFailed(device->CreateCommittedResource(
        &bufferType,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(intermediateBuffer.GetAddressOf())));

    return intermediateBuffer;
}


D3D12_SUBRESOURCE_DATA SpecifyResourceData(const void* pData, u64 byteSize)
{
    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = pData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = byteSize;

    return subResourceData;
}

CD3DX12_RESOURCE_BARRIER StaticBuffer::GetCopyTransition()
{
    return CD3DX12_RESOURCE_BARRIER::Transition(_MainBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
}

CD3DX12_RESOURCE_BARRIER StaticBuffer::GetReadTransition()
{
    return CD3DX12_RESOURCE_BARRIER::Transition(_MainBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void StaticBuffer::Create(ID3D12Device* device, u64 byteSize)
{
    _ByteSize = byteSize;
    _MainBuffer = CreateStaticBuffer(device, byteSize);
    _IntermediateBuffer = CreateIntermediateBuffer(device, byteSize);
}

void StaticBuffer::Upload(ID3D12GraphicsCommandList* cmdList, const void* data)
{
    auto copyTransition = GetCopyTransition();
    cmdList->ResourceBarrier(1, &copyTransition);

    // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
    // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
    // the intermediate upload heap data will be copied to mBuffer.
    auto subResourceData = SpecifyResourceData(data, _ByteSize);
    UpdateSubresources<1>(cmdList, _MainBuffer.Get(), _IntermediateBuffer.Get(), 0, 0, 1, &subResourceData);

    auto readTransition = GetReadTransition();
    cmdList->ResourceBarrier(1, &readTransition);
}

// After the command list executes the CopySubresourceRegion command, the buffers can be disposed.
void StaticBuffer::Dispose()
{
    IBuffer::Dispose();
    _IntermediateBuffer.Reset();
}