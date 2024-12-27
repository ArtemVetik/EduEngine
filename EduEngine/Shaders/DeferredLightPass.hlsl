#include "LightingUtil.hlsl"

Texture2D gAlbedoTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gMaterialTexture : register(t2);
Texture2D gDepthTexture : register(t3);
StructuredBuffer<Light> gLights : register(t4);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

cbuffer cbPass : register(b0)
{
    float4x4 gProjInv;
    float4x4 gViewInv;
    float3 gEyePosW;
    uint gDirectionalLightsCount;
    uint gPointLightsCount;
    uint gSpotLightsCount;
    float2 gPadding;
    float4 gClearColor;
    float4 gAmbientLight;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float2 TexC : TEXCOORD;
};
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vIn)
{
    VertexOut vOut;
    vOut.PosH = float4(vIn.PosL, 1.0f);
    vOut.TexC = vIn.TexC;
    return vOut;
}

[earlydepthstencil]
float4 PS(VertexOut pIn) : SV_TARGET
{
    float z = gDepthTexture.Sample(gsamPointWrap, pIn.TexC).r;
    
    if (z >= 1.0f)
        return gClearColor;
    
    float4 diffuseAlbedo = gAlbedoTexture.Sample(gsamPointWrap, pIn.TexC);
    float4 normal = gNormalTexture.Sample(gsamPointWrap, pIn.TexC);
    float4 material = gMaterialTexture.Sample(gsamPointWrap, pIn.TexC);
	
    float4 clipSpacePosition = float4(pIn.TexC * 2 - 1, z, 1);
    clipSpacePosition.y *= -1.0f;
    float4 viewSpacePosition = mul(gProjInv, clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w;
    float4 worldSpacePosition = mul(gViewInv, viewSpacePosition);
	
    float4 posW = worldSpacePosition;
	
    float3 toEyeW = normalize(gEyePosW - posW.rgb);
	
    Material mat = { diffuseAlbedo, material.rgb, 1.0f - material.a };
	
    float3 result = 0.0f;
    
    int i = 0;
    for (; i < gDirectionalLightsCount; i++)
    {
        result += normal.a * ComputeDirectionalLight(gLights[i], mat, normal.rgb, toEyeW);
    }
    
    for (i = gDirectionalLightsCount; i < gDirectionalLightsCount + gPointLightsCount; i++)
    {
        result += ComputePointLight(gLights[i], mat, posW.rgb, normal.rgb, toEyeW);
    }
    
    for (i = gDirectionalLightsCount + gPointLightsCount; i < gDirectionalLightsCount + gPointLightsCount + gSpotLightsCount; i++)
    {
        result += ComputeSpotLight(gLights[i], mat, posW.rgb, normal.rgb, toEyeW);
    }

    float4 ambient = gAmbientLight * diffuseAlbedo;
    float4 litColor = ambient + +float4(result, 0.0f);
    litColor.a = diffuseAlbedo.a;
	
    return litColor;
}