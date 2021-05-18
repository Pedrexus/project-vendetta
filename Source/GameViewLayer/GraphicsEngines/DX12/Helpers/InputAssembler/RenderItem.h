#pragma once

#include <Helpers/Settings/Settings.h>

#include "../Buffers/InputAssembler/IMeshBuffer.h"
#include "Objects/Geometry.h"

class RenderItem
{
    // Indicates object data has changed 
    // and we need to update the constant buffer
    u32 _NumFramesDirty = Settings::GetInt("graphics-frame-resources");

    D3D12_VERTEX_BUFFER_VIEW _VertexBufferView = {};
    D3D12_INDEX_BUFFER_VIEW _IndexBufferView = {};

public:
    std::string Name;
    XMFLOAT4X4 World;

    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    
    // RenderItem can use a Static Mesh Buffer or a Dynamic Mesh Buffer
    IMeshBuffer* Buffer;
    SubmeshGeometry Submesh;

    
public:
    RenderItem(std::string name, XMMATRIX world, SubmeshGeometry* submesh, IMeshBuffer* buffer) :
        Name(name),
        Submesh(*submesh),
        Buffer(buffer)
    {
        XMStoreFloat4x4(&World, world);
    };

    RenderItem(std::string name, XMMATRIX world, IMeshBuffer* buffer, u64 indexCount) :
        Name(name),
        Submesh({ (u32) indexCount }),
        Buffer(buffer)
    {
        XMStoreFloat4x4(&World, world);
    };

    bool IsDirty() const
    {
        return _NumFramesDirty > 0;
    }
    void Clean()
    {
        _NumFramesDirty--;
    }
    void Reset()
    {
        _NumFramesDirty = Settings::GetInt("graphics-frame-resources");
    }

    inline D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const
    {
        return Buffer->GetVertexBufferView();
    }

    inline D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
    {
        return Buffer->GetIndexBufferView();
    }
};