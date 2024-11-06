
cbuffer cbPass : register(b0)
{
    float4x4 gMVP;
	float3 gCamPos;
	float padding;
};
struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float3 PosW  : POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
    vout.PosW = vin.PosL;
    vout.PosH = mul(float4(vin.PosL, 1.0f), gMVP);
	vout.Color = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float MaxDistance = 400;
    float MinDistance = 0;
    float dist = distance(pin.PosW.xyz, gCamPos.xyz);
    
    float alpha = saturate((MaxDistance - dist) / (MaxDistance - MinDistance));
    
    return float4(pin.Color.rgb, alpha);
}



