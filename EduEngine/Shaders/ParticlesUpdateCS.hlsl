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
    particle.Color = lerp(gStartColor, gEndColor, particle.Age / gLifeTime);
    
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
