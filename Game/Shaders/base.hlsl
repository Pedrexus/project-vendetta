
#include "Lighting.hlsli"

SamplerState gsamLinearClamp : register(s0);
SamplerState gsamAnisotropicWrap : register(s1);

Texture2D gDiffuseMap : register(t0);

cbuffer RenderPassConstants : register(b0)
{
    float4x4 gViewProj;
    float3 gEyePosW;
    float gTime;
};

cbuffer ObjectConstants : register(b1)
{
    float4x4 gWorld;
    float4x4 gTextureTransform;
    
    uint gNumLights;
    Light gLights[MaxLights];
};

cbuffer MaterialConstants : register(b2)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMaterialTransform;
};

struct Vertex
{
    float3 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 texcoord : TEXCOORD0;
};

VertexOut VS(Vertex In)
{
    float4 posW = mul(gWorld, float4(In.position, 1.0f));
    float4 texC = mul(gTextureTransform, float4(In.texcoord, 0.0f, 1.0f));
    
    VertexOut Out;
    
    Out.posH = mul(gViewProj, posW);
    Out.posW = posW.xyz;
    Out.normalW = mul(In.normal, (float3x3) gWorld);
    Out.texcoord = mul(gMaterialTransform, texC).xy; // TODO: try with *
    
    return Out;
}

float4 PS(VertexOut In) : SV_TARGET0
{
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, In.texcoord) * gDiffuseAlbedo;
	
    // Interpolating normal can unnormalize it, so renormalize it.
    In.normalW = normalize(In.normalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - In.posW);

    // Light terms.
    const float4 ambientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
    float4 ambient = ambientLight * diffuseAlbedo;
    
    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, gNumLights, mat, In.posW, In.normalW, toEyeW, shadowFactor);
    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}