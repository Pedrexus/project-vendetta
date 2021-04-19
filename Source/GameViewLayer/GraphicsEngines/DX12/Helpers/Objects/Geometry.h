#pragma once

#include <concepts>

#include "../../dx12pch.h"
#include "../InputAssembler/Vertex.h"

template <class T>
concept Integral = std::is_integral<T>::value;
template <class T>
concept SignedIntegral = Integral<T> && std::is_signed<T>::value;
template <class T>
concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;

struct Mesh
{
    std::vector<Vertex> Vertices;
    std::vector<u64> Indices;

    template<UnsignedIntegral T>
    std::vector<T> GetIndices()
    {
        // assert(std::numeric_limits<T>::max() > indices.size());

        std::vector<T> indicesNew;

        auto caster = [] (u64 x) { return static_cast<T>(x); };
        std::transform(Indices.begin(), Indices.end(), indicesNew.begin(), caster);

        return indicesNew;
    }
};

namespace Geometry
{
    Mesh CreateCylinder(f32 bottomRadius, f32 topRadius, f32 height, u32 sliceCount, u32 stackCount);
    Mesh CreateBox(f32 width, f32 height, f32 depth);
}