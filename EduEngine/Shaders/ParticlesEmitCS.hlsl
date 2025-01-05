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
	
    if (gShapeType == 0) // rect
    {
        emitParticle.Position = gCenterPos + randomInRect(gTotalTime * 100 + id.x, gShapeSize); 
    }
    else if (gShapeType == 1) // inside sphere
    {
        float radius = random(gTotalTime * 100 + id.x + 100, 0.0f, gShapeSize.x);
        emitParticle.Position = gCenterPos + randomUnitVector(gTotalTime * 100 + id.x) * radius;
    }
    else if (gShapeType == 2) // sphere contour
    {
        emitParticle.Position = gCenterPos + randomUnitVector(gTotalTime * 100 + id.x) * gShapeSize.x;
    }
    
    emitParticle.Velocity = gVelocity;
    emitParticle.Color = gStartColor;
    emitParticle.Age = 0.0f;
    emitParticle.Size = 0.8;
    emitParticle.Alive = 1.0f;

    ParticlePool[emitIndex] = emitParticle;
    
    GroupMemoryBarrierWithGroupSync();
    
    DeadListCounter[0] = max((int)DeadListCounter[0] - (int)gEmitCount, 0);
}