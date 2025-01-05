struct Particle
{
    float4 Color;
    float3 Position;
    float Age;
    float3 Velocity;
    float Size;
    float Alive;
    float3 Padding;
};

cbuffer passData : register(b0)
{
    float4x4 gMVP;
    float gAspectRatio;
    float gDeltaTime;
    float gTotalTime;
    uint gEmitCount;
}

cbuffer particleData : register(b1)
{
    float3 gCenterPos;
    uint gMaxParticles;
    float4 gStartColor;
    float4 gEndColor;
    float3 gVelocity;
    float gLifeTime;
    float3 gAcceleration;
    uint gPadding;
}