#include "../Geometry.h"

Mesh Geometry::CreateGeosphere(f32 radius, u32 numSubdivisions)
{
	auto data = CreateIcosahedron();

	for (u32 i = 0; i < numSubdivisions; ++i)
		Subdivide(data);

	// Project vertices onto sphere and scale.
	for (u32 i = 0; i < data.Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&data.Vertices[i].Position));

		// Project onto sphere.
		XMVECTOR p = radius * n;

		XMStoreFloat3(&data.Vertices[i].Position, p);
		XMStoreFloat3(&data.Vertices[i].Normal, n);

		// Derive texture coordinates from spherical coordinates.
		float theta = atan2f(data.Vertices[i].Position.z, data.Vertices[i].Position.x);

		// Put in [0, 2pi].
		if (theta < 0.0f)
			theta += XM_2PI;

		float phi = acosf(data.Vertices[i].Position.y / radius);

		data.Vertices[i].Texture.x = theta / XM_2PI;
		data.Vertices[i].Texture.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		data.Vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
		data.Vertices[i].Tangent.y = 0.0f;
		data.Vertices[i].Tangent.z = +radius * sinf(phi) * cosf(theta);

		XMVECTOR T = XMLoadFloat3(&data.Vertices[i].Tangent);
		XMStoreFloat3(&data.Vertices[i].Tangent, XMVector3Normalize(T));
	}

	return data;
}


void Geometry::Subdivide(Mesh& data)
{
	// Save a copy of the input geometry.
	Mesh inputCopy = data;


	data.Vertices.resize(0);
	data.Vertices.reserve(inputCopy.Vertices.size() * 2);

	data.Indices.resize(0);
	data.Indices.reserve(inputCopy.Indices.size() * 4);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	auto numTriangles = inputCopy.Indices.size() / 3;
	for (u64 i = 0; i < numTriangles; ++i)
	{
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0 = CreateMidpoint(v0, v1);
		Vertex m1 = CreateMidpoint(v1, v2);
		Vertex m2 = CreateMidpoint(v0, v2);

		//
		// Add new geometry.
		//

		data.Vertices.push_back(v0); // 0
		data.Vertices.push_back(v1); // 1
		data.Vertices.push_back(v2); // 2
		data.Vertices.push_back(m0); // 3
		data.Vertices.push_back(m1); // 4
		data.Vertices.push_back(m2); // 5

		data.Indices.push_back(i * 6 + 0);
		data.Indices.push_back(i * 6 + 3);
		data.Indices.push_back(i * 6 + 5);

		data.Indices.push_back(i * 6 + 3);
		data.Indices.push_back(i * 6 + 4);
		data.Indices.push_back(i * 6 + 5);

		data.Indices.push_back(i * 6 + 5);
		data.Indices.push_back(i * 6 + 4);
		data.Indices.push_back(i * 6 + 2);

		data.Indices.push_back(i * 6 + 3);
		data.Indices.push_back(i * 6 + 1);
		data.Indices.push_back(i * 6 + 4);
	}
}


Vertex Geometry::CreateMidpoint(const Vertex& v0, const Vertex& v1)
{
	XMVECTOR p0 = XMLoadFloat3(&v0.Position);
	XMVECTOR p1 = XMLoadFloat3(&v1.Position);

	XMVECTOR c0 = XMLoadFloat4(&v0.Color);
	XMVECTOR c1 = XMLoadFloat4(&v1.Color);

	XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
	XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

	XMVECTOR tan0 = XMLoadFloat3(&v0.Tangent);
	XMVECTOR tan1 = XMLoadFloat3(&v1.Tangent);

	XMVECTOR tex0 = XMLoadFloat2(&v0.Texture);
	XMVECTOR tex1 = XMLoadFloat2(&v1.Texture);

	// Compute the midpoints of all the attributes.  Vectors need to be normalized
	// since linear interpolating can make them not unit length.  
	XMVECTOR pos = 0.5f * (p0 + p1);
	XMVECTOR color = 0.5f * (c0 + c1);
	XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
	XMVECTOR tangent = XMVector3Normalize(0.5f * (tan0 + tan1));
	XMVECTOR tex = 0.5f * (tex0 + tex1);

	Vertex v;
	XMStoreFloat3(&v.Position, pos);
	XMStoreFloat4(&v.Color, color);
	XMStoreFloat3(&v.Normal, normal);
	XMStoreFloat3(&v.Tangent, tangent);
	XMStoreFloat2(&v.Texture, tex);

	return v;
}