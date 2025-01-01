#define MAX_CASCADES 4

Texture2D gDiffuseMap : register(t0);
Texture2D gShadowMap[MAX_CASCADES] : register(t1);

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
    float4x4 gView;
    float3 gEyePosW;
    uint gCascadeCount;
    float4x4 gCascadeTransform[MAX_CASCADES];
    float4 gCascadeDistance;
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
    float3 PosW     : POSITION1;
    float  Depth    : POSITION2;
    float3 NormalW  : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC     : TEXCOORD;
};

struct PSOut
{
    float4 albedo   : SV_TARGET0;
    float4 normal   : SV_TARGET1;
    float4 material : SV_TARGET2;
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

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    vout.PosW = posW.xyz;
    vout.PosH = mul(posW, gViewProj);
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
    vout.TangentW = mul(vin.TangentU, (float3x3) gWorld);
    vout.TexC = vin.TexC;
    vout.Depth = mul(mul(float4(vin.PosL, 1.0f), gWorld), gView).z;
	
    return vout;
}

PSOut PS(VertexOut pin) : SV_Target
{
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLinearWrap, pin.TexC) * gDiffuseAlbedo;
    
    int4 comparisons = int4(pin.Depth >= gCascadeDistance.r,
                            pin.Depth >= gCascadeDistance.g,
                            pin.Depth >= gCascadeDistance.b,
                            pin.Depth >= gCascadeDistance.a);
    
    int cascadeIndex = min(
        MAX_CASCADES - 1, 
        dot(comparisons, int4(gCascadeCount > 0, gCascadeCount > 1, gCascadeCount > 2, gCascadeCount > 3))
    );
    
    float4 shadowPosH = mul(float4(pin.PosW, 1), gCascadeTransform[cascadeIndex]);
    float shadowFactor = CalcShadowFactor(shadowPosH, cascadeIndex);
    
    float depthExceedsMax = step(gCascadeDistance.a, pin.Depth);
    shadowFactor = lerp(shadowFactor, 1.0f, depthExceedsMax);
    
    PSOut output;
    output.albedo = diffuseAlbedo;
    output.normal = float4(normalize(pin.NormalW), shadowFactor);
    output.material = float4(gFresnelR0, gRoughness);
    
    return output;
}

#undef MAX_CASCADES