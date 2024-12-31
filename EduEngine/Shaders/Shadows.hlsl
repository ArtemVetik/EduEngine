cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
    float4x4 gViewProj;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    
    vout.PosH = mul(posW, gViewProj);
    vout.TexC = vin.TexC;
	
    return vout;
}


