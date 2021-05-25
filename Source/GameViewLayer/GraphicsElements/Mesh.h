#pragma once

#include <Helpers/Concepts/concepts.h>

#include "Vertex.h"

struct Mesh
{
    std::vector<Vertex> Vertices;
    std::vector<u64> Indices;

    inline u64 GetVerticesByteSize() const
    {
        return Vertices.size() * sizeof(Vertex);
    }

    template<UnsignedIntegral T>
    inline u64 GetIndicesByteSize() const
    {
        return Indices.size() * sizeof(T);
    }

    template<UnsignedIntegral T>
    std::vector<T> GetIndices() const
    {
        assert((std::numeric_limits<T>::max)() > Indices.size());

        std::vector<T> indicesNew;
        indicesNew.resize(Indices.size());

        auto caster = [] (u64 x) { return static_cast<T>(x); };
        std::transform(Indices.begin(), Indices.end(), indicesNew.begin(), caster);

        return indicesNew;
    }
};