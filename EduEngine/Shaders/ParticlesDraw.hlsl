#include "ParticlesInclude.hlsl"

struct VS_OUTPUT
{
    float3 Position : POSITION;
    float3 Size      : SIZE;
    float4 Color    : COLOR;
};

struct GS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color    : COLOR;
    float2 UV       : TEXCOORD;
};

StructuredBuffer<Particle> ParticlePool : register(t0);
StructuredBuffer<uint> DrawList		    : register(t1);
Texture2D ColorTexture	                : register(t2);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

VS_OUTPUT VS(uint id : SV_VertexID)
{
	VS_OUTPUT output;

    uint drawIndex = DrawList.Load(id);
	Particle particle = ParticlePool.Load(drawIndex);
	
	output.Position = particle.Position;
	output.Size = particle.Size;
	output.Color = particle.Color;

	return output;
}

[maxvertexcount(4)]
void GS(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> outStream)
{
    GS_OUTPUT output;

    float2 offsets[4];
    offsets[0] = float2(-1.0f, -1.0f);
    offsets[1] = float2(-1.0f, +1.0f);
    offsets[2] = float2(+1.0f, -1.0f);
    offsets[3] = float2(+1.0f, +1.0f);
    
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        output.Position = mul(float4(input[0].Position, 1.0f), gMVP);

        float depthChange = output.Position.z / output.Position.w;
        
        offsets[i].y *= gAspectRatio;
        output.Position.xy += offsets[i] * input[0].Size.xy * depthChange;
        output.Color = input[0].Color;
        output.UV = saturate(offsets[i]);
        
        outStream.Append(output);
    }
}

float4 PS(GS_OUTPUT input) : SV_TARGET
{
    return ColorTexture.Sample(gsamLinearWrap, input.UV) * input.Color;
}