#pragma once

#include <GameViewLayer/GraphicsElements/Mesh.h>

namespace Geometry
{
    Mesh CreateCylinder(f32 bottomRadius, f32 topRadius, f32 height, u32 sliceCount, u32 stackCount);
    Mesh CreateBox(f32 width, f32 height, f32 depth);
    Mesh CreateSphere(f32 radius, u32 sliceCount, u32 stackCount);
    Mesh CreateIcosahedron();
    Mesh CreateGeosphere(f32 radius, u32 numSubdivisions);
    Mesh CreateGrid(f32 width, f32 depth, u32 m, u32 n);

    namespace Special
    {
        Mesh CreateLandGrid(f32 width, f32 depth, u32 m, u32 n);
    }

    void Subdivide(Mesh& data);
    Vertex CreateMidpoint(const Vertex& v0, const Vertex& v1);
}