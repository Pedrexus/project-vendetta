#pragma once

#include <dx12pch.h>

#include "../../InputAssembler/Objects/Geometry.h"

class IMeshBuffer
{
protected:
	using Index = u16;

public:
	virtual D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const = 0;
	virtual D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const = 0;
};