TextureCube gCubeMap : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

cbuffer cb : register(b0)
{
    float4x4 gWorld;
    float4x4 gViewProj;
    float3 gEyePosW;
    uint gPadding;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosL = vin.PosL;
	
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	posW.xyz += gEyePosW;
	vout.PosH = mul(posW, gViewProj).xyww;
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gCubeMap.Sample(gsamLinearWrap, pin.PosL);
}

