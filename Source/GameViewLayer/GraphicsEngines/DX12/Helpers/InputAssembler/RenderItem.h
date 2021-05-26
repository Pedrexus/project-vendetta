#pragma once

#include <Helpers/Settings/Settings.h>
#include <GameViewLayer/GraphicsElements/Object.h>

#include "../Buffers/InputAssembler/IMeshBuffer.h"
#include "Objects/Geometry.h"

struct RenderItem : public Object
{
    // RenderItem can use a Static Mesh Buffer or a Dynamic Mesh Buffer
    IMeshBuffer* Buffer;
    SubmeshGeometry Submesh;

    // TODO: should pass and Object& instead of all this
    RenderItem(Object& obj, SubmeshGeometry* submesh, IMeshBuffer* buffer) :
        Object(obj),
        Submesh(*submesh),
        Buffer(buffer)
    {};

    RenderItem(Object& obj, IMeshBuffer* buffer, u64 indexCount) :
        Object(obj),
        Submesh({ (u32) indexCount }),
        Buffer(buffer)
    {};

    inline D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const
    {
        return Buffer->GetVertexBufferView();
    }

    inline D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
    {
        return Buffer->GetIndexBufferView();
    }
};