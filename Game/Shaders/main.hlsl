//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#include "Lighting.hlsli"

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
    Light gLights[32];
};

cbuffer cbPerPass : register(b1)
{
    float4x4 gViewProj;
    float3 gEyePosW;
    float gTime;
};

cbuffer cbMaterial : register(b2)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMatTransform;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 Texture : TEXCOORD;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 Texture : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	// vin.PosL.y += 5.0f * exp(-pow(vin.PosL.x - 2.0f *sin(gTime / 2.0f), 2.0f) - pow(vin.PosL.z - 2.0f * cos(gTime / 2.0f), 2.0f));
	// vout.Color = vin.Color + vin.PosL.y / 10.0f;

    float4 posW = mul(gWorld, float4(vin.PosL, 1.0f));
    float4 texC = mul(float4(vin.Texture, 0.0f, 1.0f), gTexTransform);
    
    VertexOut vout;
    vout.PosW = posW.xyz;
    vout.PosH = mul(gViewProj, posW);
    vout.NormalW = mul(vin.Normal, (float3x3) gWorld);
    vout.Texture = mul(texC, gMatTransform).xy;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
    float4 diffuseAlbedo = gDiffuseAlbedo;
	
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    // Light terms.
    const float4 ambientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    
    
    float4 ambient = ambientLight * diffuseAlbedo;
    
    Light light =
    {
        {
            0.9f, 0.9f, 0.9f
        },
        1.0f,
        {
            0.57735f, -0.57735f, 0.57735f
        },
        10.0f,
        {
            0.0f, 0.0f, 0.0f

        },
        64.0f
    };
    Light lights[16];
    lights[0] = light;

    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(lights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);
    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


