#pragma once

#include <Helpers/Concepts/concepts.h>

struct Mesh
{
    using Vertex = GeometricPrimitive::VertexType;
    using Index = u16;

    std::vector<Vertex> Vertices;
    std::vector<Index> Indices;

    inline u64 GetVerticesByteSize() const
    {
        return Vertices.size() * sizeof(Vertex);
    }

    inline u64 GetIndicesByteSize() const
    {
        return Indices.size() * sizeof(Index);
    }
};