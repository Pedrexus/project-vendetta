#include "../Geometry.h"

Mesh Geometry::CreateSphere(f32 radius, u32 sliceCount, u32 stackCount)
{
	Mesh data;

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	data.Vertices.push_back(topVertex);

	auto phiStep = XM_PI / stackCount;
	auto thetaStep = 2.0f * XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (u32 i = 1; i <= stackCount - 1; ++i)
	{
		auto phi = i * phiStep;

		// Vertices of ring.
		for (u32 j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;

			// spherical to cartesian
			v.Position.x = radius * sinf(phi) * cosf(theta);
			v.Position.y = radius * cosf(phi);
			v.Position.z = radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			v.Tangent.x = -radius * sinf(phi) * sinf(theta);
			v.Tangent.y = 0.0f;
			v.Tangent.z = +radius * sinf(phi) * cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.Tangent);
			XMStoreFloat3(&v.Tangent, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.Position);
			XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

			v.Texture.x = theta / XM_2PI;
			v.Texture.y = phi / XM_PI;

			data.Vertices.push_back(v);
		}
	}

	data.Vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (u64 i = 1; i <= sliceCount; ++i)
	{
		data.Indices.push_back(0);
		data.Indices.push_back(i + 1);
		data.Indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	u64 baseIndex = 1;
	u32 ringVertexCount = sliceCount + 1;
	for (u64 i = 0; i < (u64) stackCount - 2; ++i)
	{
		for (u32 j = 0; j < sliceCount; ++j)
		{
			data.Indices.push_back(baseIndex + i * ringVertexCount + j);
			data.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			data.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			data.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	auto southPoleIndex = data.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (u32 i = 0; i < sliceCount; ++i)
	{
		data.Indices.push_back(southPoleIndex);
		data.Indices.push_back(baseIndex + i);
		data.Indices.push_back(baseIndex + i + 1);
	}

	return data;
}
