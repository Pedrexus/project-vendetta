#pragma once

#include "IRendered.h"
#include "Light.h"

static constexpr auto MAX_LIGHTS = 32;

struct ObjectConstants
{
    XMFLOAT4X4 World;
    Light Lights[MAX_LIGHTS]; // lights that reach the object
};

struct Object : public IRendered, public ObjectConstants
{
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // RenderItem can use a Static Mesh Buffer or a Dynamic Mesh Buffer
    // IMeshBuffer* Buffer;
    // SubmeshGeometry Submesh;

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

    inline D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const
    {
        return Buffer->GetVertexBufferView();
    }

    inline D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
    {
        return Buffer->GetIndexBufferView();
    }
};