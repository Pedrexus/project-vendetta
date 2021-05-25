//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbPerPass : register(b1)
{
	float4x4 gViewProj;
	float gTime;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// vin.PosL.y += 5.0f * exp(-pow(vin.PosL.x - 2.0f *sin(gTime / 2.0f), 2.0f) - pow(vin.PosL.z - 2.0f * cos(gTime / 2.0f), 2.0f));
	// vout.Color = vin.Color + vin.PosL.y / 10.0f;

	// Transform to homogeneous clip space.
	float4 posW = mul(gWorld, float4(vin.PosL, 1.0f));
	vout.PosH = mul(gViewProj, posW);

	// Just pass vertex color into the pixel shader.
	vout.Color = vin.Color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}


