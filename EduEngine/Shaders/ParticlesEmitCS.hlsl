#include "SimpleRandom.hlsl"
#include "ParticlesInclude.hlsl"

RWStructuredBuffer<Particle> ParticlePool : register(u0);
ConsumeStructuredBuffer<uint> CDeadList : register(u1);
RWStructuredBuffer<uint> DeadListCounter : register(u4);

[numthreads(32, 1, 1)]
void main(uint id : SV_DispatchThreadID)
{
    if (id.x >= min(gEmitCount, DeadListCounter[0]))
        return;
    
    uint emitIndex = CDeadList.Consume();
	
    Particle emitParticle = ParticlePool.Load(emitIndex);
	
    if (emitParticle.Alive != 0.0f)
        return;
	
    emitParticle.Position = gCenterPos + float3(random(gTotalTime * 100 + id.x, -15, 15), 0, random(gTotalTime * 100 + id.x + 1, -15, 15));
    emitParticle.Velocity = gVelocity;
    emitParticle.Color = gStartColor;
    emitParticle.Age = 0.0f;
    emitParticle.Size = 0.8;
    emitParticle.Alive = 1.0f;

    ParticlePool[emitIndex] = emitParticle;
    
    GroupMemoryBarrierWithGroupSync();
    
    DeadListCounter[0] = max((int)DeadListCounter[0] - (int)gEmitCount, 0);
}