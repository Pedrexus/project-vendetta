
Texture2D gDiffuseMap : register(t0);

SamplerState gsamLinearClamp : register(s0);
SamplerState gsamAnisotropicWrap : register(s1);

cbuffer RenderPass : register(b0)
{
    float4x4 gViewProj;
};

cbuffer Object : register(b1)
{
    float4x4 gWorld;
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
    
    VertexOut Out;
    
    Out.posH = mul(gViewProj, posW);
    Out.posW = posW.xyz;
    Out.normalW = mul(In.normal, (float3x3) gWorld);
    Out.texcoord = In.texcoord; // mul gTexTransform, gMatTransform
    
    return Out;
}

float4 PS(VertexOut In) : SV_TARGET0
{
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, In.texcoord);
    
    return diffuseAlbedo;
}