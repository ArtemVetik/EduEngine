#include "ParticlesInclude.hlsl"

AppendStructuredBuffer<uint> ADeadList : register(u1);
RWStructuredBuffer<uint> DeadListCounter : register(u4);

[numthreads(32, 1, 1)]
void main(uint id : SV_DispatchThreadID)
{
    if (id.x >= gMaxParticles) 
        return;
	
    ADeadList.Append(id.x);
    
    InterlockedAdd(DeadListCounter[0], 1);
}