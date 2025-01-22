#define MAX_CASCADES 4

#include "LightingUtil.hlsl"

Texture2D gAlbedoTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gMaterialTexture : register(t2);
Texture2D gDepthTexture : register(t3);
TextureCube gCubeMap : register(t4);
StructuredBuffer<Light> gLights : register(t5);
Texture2D gShadowMap[MAX_CASCADES] : register(t6);

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
    float4x4 gView;
    float3 gEyePosW;
    uint gDirectionalLightsCount;
    uint gPointLightsCount;
    uint gSpotLightsCount;
    uint gCascadeCount;
    uint gPadding;
    float4 gClearColor;
    float4 gAmbientLight;
    float4x4 gCascadeTransform[MAX_CASCADES];
    float4 gCascadeDistance;
};

cbuffer cbCameraViewport : register(b1)
{
    float4 gViewport;
}

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

//---------------------------------------------------------------------------------------
// PCF for shadow mapping.
//---------------------------------------------------------------------------------------

float CalcShadowFactor(float4 shadowPosH, uint mapIndex)
{
    shadowPosH.xyz /= shadowPosH.w;
    
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap[mapIndex].GetDimensions(0, width, height, numMips);
	
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };
	
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += gShadowMap[mapIndex].SampleCmpLevelZero(gsamShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }
	
    return percentLit / 9.0f;
}

VertexOut VS(VertexIn vIn)
{
    VertexOut vOut;
    vOut.PosH = float4(vIn.PosL, 1.0f);
    vOut.TexC = gViewport.xy + vIn.TexC * gViewport.zw;
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
	
    float viewDepth = mul(posW, gView).z;
    
    int4 comparisons = int4(viewDepth >= gCascadeDistance.r,
                            viewDepth >= gCascadeDistance.g,
                            viewDepth >= gCascadeDistance.b,
                            viewDepth >= gCascadeDistance.a);
    
    int cascadeIndex = min(
        MAX_CASCADES - 1,
        dot(comparisons, int4(gCascadeCount > 0, gCascadeCount > 1, gCascadeCount > 2, gCascadeCount > 3))
    );
    
    float4 shadowPosH = mul(float4(posW.xyz, 1), gCascadeTransform[cascadeIndex]);
    float shadowFactor = CalcShadowFactor(shadowPosH, cascadeIndex);
    
    
    float depthExceedsMax = step(gCascadeDistance.a, viewDepth);
    shadowFactor = lerp(shadowFactor, 1.0f, depthExceedsMax);
    
    float3 result = 0.0f;
    
    int i = 0;
    for (; i < gDirectionalLightsCount; i++)
    {
        result += shadowFactor * ComputeDirectionalLight(gLights[i], mat, normal.rgb, toEyeW);
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
    float4 litColor = ambient + float4(result, 0.0f);
    
    float3 r = reflect(-toEyeW, normal.xyz);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(material.rgb, normal.xyz, r);
    litColor.rgb += material.a * fresnelFactor * reflectionColor.rgb;
    
    litColor.a = diffuseAlbedo.a;
	
    return litColor;
}

#undef MAX_CASCADES