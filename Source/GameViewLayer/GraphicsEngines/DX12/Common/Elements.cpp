#include "Elements.h"

static const auto NormalizedOne = Vector3(XMVector3Normalize({ 1, 1, 1 }));

Material::Constants Material::Wood = {
	Vector4::One,
	Vector3(0.05f),
	0.2f,
	Matrix::Identity,
};

Material::Constants Material::Ivory = {
	Vector4(0.7f),
	Vector3(0.05f),
	0.7f,
	Matrix::Identity,
};

const Light::Constants Light::DirectionalDefault = {
	Vector3::Zero,
	1.0f,
	Vector3(XMVector3Normalize({ 1, -1, 1 })),
	10.0f,
	Vector3::One,
	64.0f,
	Directional,
};

const Light::Constants Light::PointDefault = {
	Vector3(-5.0f),
	1.0f,
	{},
	10.0f,
	Vector3::One,
	{},
	Point,
};

const Light::Constants Light::SpotlightDefault = {
	Vector3(5.0f),
	1.0f,
	-NormalizedOne,
	10.0f,
	Vector3::One,
	{},
	Spotlight,
};