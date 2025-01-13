#include "ParticlesInclude.hlsl"

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

RWStructuredBuffer<Particle> ParticlePool : register(u0);
AppendStructuredBuffer<uint> ADeadList : register(u1);
RWStructuredBuffer<uint> DrawList : register(u2);
RWStructuredBuffer<uint> DrawArgs : register(u3);
RWStructuredBuffer<uint> DeadListCounter : register(u4);

Texture2D gDepthTexture : register(t0);
Texture2D gNormalTexture : register(t1);

[numthreads(32, 1, 1)]
void main(uint id : SV_DispatchThreadID)
{
    if (id.x >= gMaxParticles)
        return;

    Particle particle = ParticlePool.Load(id.x);

    if (particle.Alive == 0.0f)
        return;

    particle.Age += gDeltaTime;
	
    particle.Alive = (float) (particle.Age < gLifeTime);
    
    particle.Velocity += gAcceleration * gDeltaTime;
    particle.Position += particle.Velocity * gDeltaTime;
    
    float normAge = particle.Age / gLifeTime;
    
    if (length(particle.Velocity) > 0)
        particle.Velocity += -normalize(particle.Velocity) * step(normAge, gDragTime) * gDragForce * gDeltaTime;
     
    particle.Color = lerp(particle.StartColor, gEndColor, normAge);
    particle.Size = lerp(particle.StartSize, gEndSize, normAge);
    
#if SCREEN_SPACE_COLLISION
    float3 viewPos = mul(float4(particle.Position, 1.0f), gWorldView).xyz;
	float4 clipSpacePos = mul(float4(viewPos, 1.0f), gProj);
	clipSpacePos /= clipSpacePos.w;
	float2 aClipSpacePos = abs(clipSpacePos);
	
	if (aClipSpacePos.x < 1.0f && aClipSpacePos.y < 1.0f)
	{
		float2 uv = clipSpacePos.xy * float2(0.5f, -0.5f) + 0.5;
        float depthValue = gDepthTexture.SampleLevel(gsamPointWrap, uv, 0).r;
        float3 normalValue = gNormalTexture.SampleLevel(gsamPointWrap, uv, 0);
		
        float linearEyeDepth = gNear * gFar / (depthValue * (gNear - gFar) + gFar);

		float radius = particle.Size;
		float surfaceThickness = 10.0f;

        if (viewPos.z > linearEyeDepth - radius && viewPos.z < linearEyeDepth + radius + surfaceThickness)
		{
            particle.Velocity = normalize(normalValue) * length(particle.Velocity);
        }
	}
#endif
    
    ParticlePool[id.x] = particle;
	
    if (particle.Alive == 0.0f)
    {
        ADeadList.Append(id.x);
        InterlockedAdd(DeadListCounter[0], 1);
    }
    else
    {
        uint drawIndex = DrawList.IncrementCounter();
        DrawList[drawIndex] = id.x;
    }
}
