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
		unsigned int MaxParticles;
		unsigned int ShapeType;
		float LifeTime;
		float EmissionRate;
		Vector3 ShapeSize;
		Vector4 StartColor;
		Vector4 EndColor;
		Vector3 Velocity;
		Vector3 Acceleration;
		bool ScreenSpaceCollision;
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