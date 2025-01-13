struct Particle
{
    float4 StartColor;
    float4 Color;
    float3 Position;
    float Age;
    float3 Velocity;
    float3 StartSize;
    float3 Size;
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
    float3 gCenterPos;
    float gPadding1;
}

cbuffer particleData : register(b1)
{
    uint gShapeType;
    float3 gShapeSize;
    uint gMaxParticles;
    float3 gMinStartSize;
    float gLifeTime;
    float3 gMaxStartSize;
    float gDragForce;
    float3 gEndSize;
    float4 gMinStartColor;
    float4 gMaxStartColor;
    float4 gEndColor;
    float3 gMinStartVelocity;
    bool gRandVelocityOnBound;
    float3 gMaxStartVelocity;
    float gDragTime;
    float3 gAcceleration;
    uint gPadding3;
}