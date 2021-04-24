#pragma once

#include <const.h>

#include "../../dx12pch.h"
#include "../DX12InputAssembler.h"

class RenderItem
{
    // Indicates object data has changed 
    // and we need to update the constant buffer
    int _NumFramesDirty = NUMBER_FRAME_RESOURCES;

public:
    // Position, Orientation and Scale of object in the World;
    XMMATRIX World = XMMatrixIdentity();

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    UINT ObjCBIndex = -1;

    MeshGeometry* Geo = nullptr;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;

    RenderItem() = default;
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
};