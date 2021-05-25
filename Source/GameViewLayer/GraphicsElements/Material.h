#pragma once

#include "IRendered.h"

struct MaterialConstants
{
	XMFLOAT4 DiffuseAlbedo;
	
	XMFLOAT3 FresnelR0;
	f32 Roughness;

	XMFLOAT4X4 Transform;
};

struct Material : public IRendered, public MaterialConstants
{
	u32 MatCBIndex = -1;
	u32 DiffuseSRVHeapIndex = -1;
};

