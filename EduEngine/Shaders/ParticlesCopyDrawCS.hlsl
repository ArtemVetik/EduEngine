#include "ParticlesInclude.hlsl"

RWStructuredBuffer<Particle> ParticlePool : register(u0);
AppendStructuredBuffer<uint> ADeadList : register(u1);
RWStructuredBuffer<uint> DrawList : register(u2);
RWStructuredBuffer<uint> DrawArgs : register(u3);
RWStructuredBuffer<uint> DeadListCounter : register(u4);

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	// increment the counter to get the previous value, which happens to be how many particles we want to draw
    DrawArgs[0] = DrawList.IncrementCounter(); // vertexCountPerInstance (or index count if using an index buffer)
	DrawArgs[1] = 1; // instanceCount
    DrawArgs[2] = 0; // offsets
	DrawArgs[3] = 0; // offsets
	DrawArgs[4] = 0; // offsets
	DrawArgs[5] = 0; // offsets
	DrawArgs[6] = 0; // offsets
	DrawArgs[7] = 0; // offsets
	DrawArgs[8] = 0; // offsets
}