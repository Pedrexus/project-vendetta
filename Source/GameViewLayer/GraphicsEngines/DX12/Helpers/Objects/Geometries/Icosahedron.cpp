#include "../Geometry.h"

Mesh Geometry::CreateIcosahedron()
{
	Mesh data;

	const float X = 0.525731f;
	const float Z = 0.850651f;

	data.Vertices =
	{
		Vertex({-X, 0.0f, Z}, XMFLOAT4(Colors::AliceBlue)),  
		Vertex({+X, 0.0f, Z}, XMFLOAT4(Colors::Aqua)),
		Vertex({-X, 0.0f, -Z}, XMFLOAT4(Colors::BlueViolet)),
		Vertex({X, 0.0f, -Z}, XMFLOAT4(Colors::CadetBlue)),
		Vertex({0.0f, Z, X}, XMFLOAT4(Colors::DarkSlateBlue)),
		Vertex({0.0f, Z, -X}, XMFLOAT4(Colors::CornflowerBlue)),
		Vertex({0.0f, -Z, X}, XMFLOAT4(Colors::DodgerBlue)),
		Vertex({0.0f, -Z, -X}, XMFLOAT4(Colors::DeepSkyBlue)),
		Vertex({Z, X, 0.0f}, XMFLOAT4(Colors::Aquamarine)),
		Vertex({-Z, X, 0.0f}, XMFLOAT4(Colors::LightBlue)),
		Vertex({Z, -X, 0.0f}, XMFLOAT4(Colors::MediumBlue)),
		Vertex({-Z, -X, 0.0f}, XMFLOAT4(Colors::RoyalBlue)),
	};

	data.Indices =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	return data;
}