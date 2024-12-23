Texture2D gDiffuseMap : register(t0);

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

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);

    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
    vout.TangentW = mul(vin.TangentU, (float3x3) gWorld);
    vout.TexC = vin.TexC;
	
    return vout;
}

PSOut PS(VertexOut pin) : SV_Target
{
    pin.NormalW = normalize(pin.NormalW);
	
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLinearWrap, pin.TexC) * gDiffuseAlbedo;

    PSOut output;
	output.albedo = diffuseAlbedo;
	output.normal = float4(pin.NormalW, 1);
    output.material = float4(gFresnelR0, gRoughness);
    
	return output;
}


