#include "LightingUtil.hlsl"

Texture2D gDiffuseMap : register(t0);
StructuredBuffer<Light> gLights : register(t1);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMatTransform;
};

cbuffer cbPass : register(b2)
{
    float4x4 gViewProj;
    float3 gEyePosW;
    uint gDirectionalLightsCount;
    uint gPointLightsCount;
    uint gSpotLightsCount;
    float2 gPadding;
    float4 gAmbientLight;
};

struct VertexIn
{
	float3 PosL     : POSITION;
    float3 NormalL  : NORMAL;
	float3 TangentU : TANGENT;
	float2 TexC     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;
	vout.PosH = mul(posW, gViewProj);

    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);
	vout.TexC = vin.TexC;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
#if NO_DIFFUSE_MAP
    float4 diffuseAlbedo = gDiffuseAlbedo;
#else
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamPointWrap, pin.TexC) * gDiffuseAlbedo;    
#endif

    pin.NormalW = normalize(pin.NormalW);

    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    float4 ambient = gAmbientLight * diffuseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    
    float3 result = 0.0f;
    
    for (int i = 0; i < gDirectionalLightsCount; i++)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, pin.NormalW, toEyeW);
    }
    
    for (int i = gDirectionalLightsCount; i < gDirectionalLightsCount + gPointLightsCount; i++)
    {
        result += ComputePointLight(gLights[i], mat, pin.PosW, pin.NormalW, toEyeW);
    }
    
    for (int i = gDirectionalLightsCount + gPointLightsCount; i < gDirectionalLightsCount + gPointLightsCount + gSpotLightsCount; i++)
    {
        result += ComputeSpotLight(gLights[i], mat, pin.PosW, pin.NormalW, toEyeW);
    }
    
    float4 litColor = ambient + float4(result, 0.0f);

    litColor.a = diffuseAlbedo.a;

    return litColor;
}


