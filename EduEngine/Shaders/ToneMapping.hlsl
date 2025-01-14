Texture2D gAccumTexture : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

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
    float2 TexC : TEXCOORD0;
};

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
    float4 hdrColor = gAccumTexture.Sample(gsamPointWrap, pIn.TexC);
    
    float gamma = 2.2;
  
    // reinhard tone mapping
    float3 mapped = hdrColor / (hdrColor + (1.0f));
    mapped = pow(mapped, (1.0 / gamma));
  
    return float4(mapped, 1.0);
}