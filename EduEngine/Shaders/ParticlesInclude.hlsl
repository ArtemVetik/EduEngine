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
    float4x4 gWorldView;
    float4x4 gProj;
    float4x4 gMVP;
    float gAspectRatio;
    float gDeltaTime;
    float gTotalTime;
    uint gEmitCount;
    uint gRandSeed;
    float gNear;
    float gFar;
    float gPadding0;
}

cbuffer particleData : register(b1)
{
    uint gShapeType;
    float3 gShapeSize;
    float3 gCenterPos;
    uint gMaxParticles;
    float4 gStartColor;
    float4 gEndColor;
    float3 gVelocity;
    float gLifeTime;
    float3 gAcceleration;
    uint gPadding1;
}