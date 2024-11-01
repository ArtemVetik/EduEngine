
cbuffer cbPass : register(b0)
{
    float4x4 gViewProj;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld;
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
    return float4(pin.TexC, 0, 0);
}


