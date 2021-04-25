#pragma once

#include <const.h>

#include "../../dx12pch.h"
#include "Objects/Geometry.h"

class RenderItem
{
    // Indicates object data has changed 
    // and we need to update the constant buffer
    int _NumFramesDirty = NUMBER_FRAME_RESOURCES;
public:
    // Position, Orientation and Scale of object in the World;
    u32 CBIndex;
    XMFLOAT4X4 World;
    SubmeshGeometry* Submesh;

    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    
public:
    RenderItem(u32 index, XMMATRIX world, SubmeshGeometry& submesh) :
        CBIndex(index),
        Submesh(&submesh)
    {
        XMStoreFloat4x4(&World, world);
    };

    bool IsDirty()
    {
        return _NumFramesDirty > 0;
    }
    void Clean()
    {
        _NumFramesDirty--;
    }
    void Reset()
    {
        _NumFramesDirty = NUMBER_FRAME_RESOURCES;
    }

    // ObjectConstants GetConstants();
};