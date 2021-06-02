#include "Elements.h"

static const auto NormalizedOne = Vector3(XMVector3Normalize({ 1, 1, 1 }));

namespace Material
{
	const Element Element::Wood = {
		"wood",
		{},
		Vector4::One,
		Vector3(0.05f),
		0.2f,
		Matrix::Identity,
	};
}

namespace Light
{
	const Element Element::DirectionalDefault = {
		"directional default",
		{},
		Vector3(0.5f),
		1.0f,
		Vector3(XMVector3Normalize({ 1, -1, 1 })),
		10.0f,
		Vector3::One,
		64.0f,
		Directional,
	};

	const Element Element::PointDefault = {
		"point default",
		{},
		Vector3(-5.0f),
		1.0f,
		{},
		10.0f,
		Vector3::One,
		{},
		Point,
	};

	//static const Element Element::PointDefault = {};
	//static const Element Element::SpotlightDefault = {};
}