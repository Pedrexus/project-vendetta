#include "../Geometry.h"

namespace Cylinder
{
	// Cylinder can be parameterized as follows, where we introduce v
	// parameter that goes in the same direction as the v tex-coord
	// so that the bitangent goes in the same direction as the v tex-coord.
	//   Let r0 be the bottom radius and let r1 be the top radius.
	//   y(v) = h - hv for v in [0,1].
	//   r(v) = r1 + (r0-r1)v
	//
	//   x(t, v) = r(v)*cos(t)
	//   y(t, v) = h - hv
	//   z(t, v) = r(v)*sin(t)
	// 
	//  dx/dt = -r(v)*sin(t)
	//  dy/dt = 0
	//  dz/dt = +r(v)*cos(t)
	//
	//  dx/dv = (r0-r1)*cos(t)
	//  dy/dv = -h
	//  dz/dv = (r0-r1)*sin(t)
	void GenerateVertices(f32 bottomRadius, f32 topRadius, f32 height, u32 sliceCount, u32 stackCount, Mesh& data)
	{

		std::array<f32, 3> center = { .0f, height / 2.0f, .0f };

		f32 stackHeight = height / stackCount;
		f32 radiusStep = (topRadius - bottomRadius) / stackCount;
		u32 ringCount = stackCount + 1;

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for (u32 i = 0; i < ringCount; i++)
		{
			f32 y = i * stackHeight;
			f32 r = bottomRadius + i * radiusStep;

			// vertices of ring
			float dTheta = 2.0f * XM_PI / sliceCount;
			for (u32 j = 0; j <= sliceCount; j++)
			{
				Vertex vertex;

				f32 c = cosf(j * dTheta);
				f32 s = sinf(j * dTheta);

				vertex.Position = XMFLOAT3(r * c - center[0], y - center[1], r * s - center[2]);
				vertex.Color = XMFLOAT4(Colors::CornflowerBlue);

				vertex.Texture.x = (float) j / sliceCount;
				vertex.Texture.y = 1.0f - (float) i / stackCount;

				// This is unit length.
				vertex.Tangent = XMFLOAT3(-s, 0.0f, c);

				float dr = bottomRadius - topRadius;
				XMFLOAT3 bitangent(dr * c, -height, dr * s);

				XMVECTOR T = XMLoadFloat3(&vertex.Tangent);
				XMVECTOR B = XMLoadFloat3(&bitangent);
				XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				XMStoreFloat3(&vertex.Normal, N);

				data.Vertices.push_back(vertex);
			}
		}
	}

	void GenerateIndices(const u32& sliceCount, const u32& stackCount, Mesh& data)
	{
		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		u32 ringVertexCount = sliceCount + 1;

		// Compute indices for each stack.
		for (u64 i = 0; i < stackCount; ++i)
		{
			for (u64 j = 0; j < sliceCount; ++j)
			{
				auto A = i * ringVertexCount + j;
				auto B = (i + 1) * ringVertexCount + j;
				auto C = (i + 1) * ringVertexCount + j + 1;
				auto D = i * ringVertexCount + j + 1;

				data.Indices.push_back(A);
				data.Indices.push_back(B);
				data.Indices.push_back(C);

				data.Indices.push_back(A);
				data.Indices.push_back(C);
				data.Indices.push_back(D);
			}
		}
	}

	enum class Cap
	{
		Top,
		Bottom,
	};

	void GenerateCap(Cap cap, f32 radius, f32 height, u32 sliceCount, u32 stackCount, Mesh& data)
	{
		auto d = cap == Cap::Top ? +1.0f : -1.0f;

		auto baseIndex = data.Vertices.size();

		auto y = d * 0.5f * height;
		auto dTheta = 2.0f * XM_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (u32 i = 0; i <= sliceCount; ++i)
		{
			auto x = radius * cosf(i * dTheta);
			auto z = radius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			auto u = x / height + 0.5f;
			auto v = z / height + 0.5f;

			auto vertex = Vertex(x, y, z, 0.0f, d * 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v);
			vertex.Color = XMFLOAT4(Colors::DodgerBlue);

			data.Vertices.push_back(vertex);
		}

		// Cap center vertex.
		auto vertex = Vertex(0.0f, y, 0.0f, 0.0f, d * 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f);
		vertex.Color = XMFLOAT4(Colors::DodgerBlue);
		data.Vertices.push_back(vertex);

		// Index of center vertex.
		auto centerIndex = data.Vertices.size() - 1;

		auto currentSize = data.Indices.size();

		for (u32 i = 0; i < sliceCount; ++i)
		{
			data.Indices.push_back(baseIndex + i);
			data.Indices.push_back(centerIndex);
			data.Indices.push_back(baseIndex + i + 1);
		}

		if (cap == Cap::Bottom)
			std::reverse(data.Indices.begin() + currentSize, data.Indices.end());
	}
}


Mesh Geometry::CreateCylinder(f32 bottomRadius, f32 topRadius, f32 height, u32 sliceCount, u32 stackCount)
{
	Mesh data = {};

	Cylinder::GenerateVertices(bottomRadius, topRadius, height, sliceCount, stackCount, data);
	Cylinder::GenerateIndices(sliceCount, stackCount, data);
	Cylinder::GenerateCap(Cylinder::Cap::Top, topRadius, height, sliceCount, stackCount, data);
	Cylinder::GenerateCap(Cylinder::Cap::Bottom, bottomRadius, height, sliceCount, stackCount, data);

	return data;
}
