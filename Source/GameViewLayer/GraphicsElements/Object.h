#pragma once

#include "IRendered.h"
#include "Light.h"
#include "Material.h"
#include "Texture.h"

static constexpr auto MAX_LIGHTS = 32;

struct ObjectConstants
{
	XMFLOAT4X4 World;
	XMFLOAT4X4 TexTransform;
	// Light Lights[MAX_LIGHTS]; // lights that reach the object
};

struct Object : public IRendered, public ObjectConstants
{
	using MaterialClass = Material;
	using TextureClass = Texture;

	std::shared_ptr<MaterialClass> Material;
	std::shared_ptr<TextureClass> Texture;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Object(std::string name, XMMATRIX world, XMMATRIX texTransform, MaterialClass& material, TextureClass& texture) :
		IRendered(name)
	{
		XMStoreFloat4x4(&World, world);
		XMStoreFloat4x4(&TexTransform, texTransform);
		Material = std::make_shared<MaterialClass>(material);
		Texture = std::make_shared<TextureClass>(texture);
	}
};