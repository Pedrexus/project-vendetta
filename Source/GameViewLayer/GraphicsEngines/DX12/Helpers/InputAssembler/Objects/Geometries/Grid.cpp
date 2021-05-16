#include "../Geometry.h"

Mesh Geometry::CreateGrid(float width, float depth, u32 m, u32 n)
{
	Mesh data;

	auto vertexCount = m * n;
	auto faceCount = (m - 1) * (n - 1) * 2;

	//
	// Create the vertices.
	//

	f32 halfWidth = 0.5f * width;
	f32 halfDepth = 0.5f * depth;

	f32 dx = width / (n - 1);
	f32 dz = depth / (m - 1);

	f32 du = 1.0f / (n - 1);
	f32 dv = 1.0f / (m - 1);

	data.Vertices.resize(vertexCount);
	for (u64 i = 0; i < m; ++i)
	{
		auto z = halfDepth - i * dz;
		for (u64 j = 0; j < n; ++j)
		{
			auto x = -halfWidth + j * dx;

			data.Vertices[i * n + j].Position = XMFLOAT3(x, 0.0f, z);
			data.Vertices[i * n + j].Color = XMFLOAT4(Colors::CornflowerBlue);
			data.Vertices[i * n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			data.Vertices[i * n + j].Tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			data.Vertices[i * n + j].Texture = { j * du, i * dv };
		}
	}

	//
	// Create the indices.
	//

	data.Indices.resize((u64) faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	u64 k = 0;
	for (u64 i = 0; i < m - 1; ++i)
	{
		for (u32 j = 0; j < n - 1; ++j)
		{
			data.Indices[k] = i * n + j;
			data.Indices[k + 1] = i * n + j + 1;
			data.Indices[k + 2] = (i + 1) * n + j;

			data.Indices[k + 3] = (i + 1) * n + j;
			data.Indices[k + 4] = i * n + j + 1;
			data.Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	return data;
}

Mesh Geometry::Special::CreateLandGrid(float width, float depth, u32 m, u32 n)
{
	Mesh data = CreateGrid(width, depth, m, n);

	for (auto& vertex : data.Vertices)
	{	
		auto& p = vertex.Position;
		p.y = .3f * (p.z * sinf(.1f * p.x) + p.x * cosf(.1f * p.z));

		if (p.y < 0.5f)
			vertex.Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f); // Sandy beach color.
		else if (p.y < 5.0f)
			vertex.Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f); // Light yellow-green.
		else if (p.y < 12.0f)
			vertex.Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f); // Dark yellow-green.
		else if (p.y < 20.0f)
			vertex.Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f); // Dark brown.
		else
			vertex.Color = XMFLOAT4(Colors::White); // White snow.
	}

	return data;
}