#pragma once
#include "TextureWrapper.h"
#include "../../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	using namespace System;
	using namespace System::Numerics;

	[Serializable]
	public value class ParticleData
	{
	public:
		float EmissionRate;
		bool ScreenSpaceCollision;
		bool EmissionEnabled;

		UINT ShapeType;
		Vector3 ShapeSize;
		UINT MaxParticles;
		Vector3 MinStartSize;
		float LifeTime;
		Vector3 MaxStartSize;
		float DragForce;
		Vector3 EndSize;
		Vector4 MinStartColor;
		Vector4 MaxStartColor;
		Vector4 EndColor;
		Vector3 MinStartVelocity;
		bool RandVelocityOnBound;
		Vector3 MaxStartVelocity;
		float DragTime;
		Vector3 Acceleration;
		float CollisionEnergyLoss;
	};

	private ref class NativeParticleSystemWrapper
	{
	public:
		NativeParticleSystemWrapper();
		~NativeParticleSystemWrapper();
		!NativeParticleSystemWrapper();

		void SetParticlesData(ParticleData% data);
		void SetCenterPosition(Vector3 position);
		void SetColorTexture(TextureWrapper^ texture);

	private:
		IParticleSystem* m_NativeParticleSystem;
	};
}