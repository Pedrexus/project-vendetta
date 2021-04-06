#pragma once

#include "IMeshGeometry.h"
#include "../InputAssembler.h"

// defines a box geometry
const struct Box : public IMeshGeometry {
	
	const InputAssembler::Vertex vertices[8] = {
		{	{ -1.0f, -1.0f, -1.0f}, XMFLOAT4(Colors::White)		},
		{	{ -1.0f, +1.0f, -1.0f}, XMFLOAT4(Colors::Black)		},
		{	{ +1.0f, +1.0f, -1.0f}, XMFLOAT4(Colors::Red)		},
		{	{ +1.0f, -1.0f, -1.0f}, XMFLOAT4(Colors::Green)		},
		{	{ -1.0f, -1.0f, +1.0f}, XMFLOAT4(Colors::Blue)		},
		{	{ -1.0f, +1.0f, +1.0f}, XMFLOAT4(Colors::Yellow)	},
		{	{ +1.0f, +1.0f, +1.0f}, XMFLOAT4(Colors::Cyan)		},
		{	{ +1.0f, -1.0f, +1.0f}, XMFLOAT4(Colors::Magenta)	}
	};

	const u16 indices[36] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
};