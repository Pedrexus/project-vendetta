#pragma once

#include "IRendered.h"
#include "Light.h"
#include "Material.h"

static constexpr auto MAX_LIGHTS = 32;

struct ObjectConstants
{
	XMFLOAT4X4 World;
	Light Lights[MAX_LIGHTS]; // lights that reach the object
};

struct Object : public IRendered
{
	using MaterialClass = Material;

	std::string Name;
	XMFLOAT4X4 World;
	std::shared_ptr<MaterialClass> Material;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Object(std::string name, XMMATRIX world, MaterialClass& material) :
		Name(name)
	{
		XMStoreFloat4x4(&World, world);
		Material = std::make_shared<MaterialClass>(material);
	}
};