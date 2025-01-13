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
        emitParticle.Position = gCenterPos + randomInRect(gRandSeed + id.x, gShapeSize);
    }
    else if (gShapeType == 1) // inside sphere
    {
        float radius = random(gRandSeed + id.x) * gShapeSize.x;
        emitParticle.Position = gCenterPos + randomUnitVector(gRandSeed + id.x + 1) * radius;
    }
    else if (gShapeType == 2) // sphere contour
    {
        emitParticle.Position = gCenterPos + randomUnitVector(gRandSeed + id.x + 2) * gShapeSize.x;
    }
    
    if (gRandVelocityOnBound)
        emitParticle.Velocity = randomUnitVector(gRandSeed + id.x + 3) * gMinStartVelocity.x;
    else
        emitParticle.Velocity = random(gRandSeed + id.x + 4, gMinStartVelocity, gMaxStartVelocity);
    
    emitParticle.StartColor = random(gRandSeed + id.x + 5, gMinStartColor, gMaxStartColor);
    emitParticle.Age = 0.0f;
    emitParticle.StartSize = random(gRandSeed + id.x + 6, gMinStartSize, gMaxStartSize);
    emitParticle.Alive = 1.0f;
    
    ParticlePool[emitIndex] = emitParticle;
    
    GroupMemoryBarrierWithGroupSync();
    
    DeadListCounter[0] = max((int) DeadListCounter[0] - (int) gEmitCount, 0);
}