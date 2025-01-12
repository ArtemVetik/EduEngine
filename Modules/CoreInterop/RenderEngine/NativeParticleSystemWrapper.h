#pragma once
#include "TextureWrapper.h"
#include "../../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	using namespace System;
	using namespace System::Numerics;

	ref class NativeParticleSystemWrapper
	{
	public:
		NativeParticleSystemWrapper();
		~NativeParticleSystemWrapper();
		!NativeParticleSystemWrapper();

		void SetMaxParticles(unsigned int num);
		void SetShapeType(UINT value);
		void SetShapeSize(Vector3 value);
		void SetCenterPosition(Vector3 value);
		void SetEmissionRate(float value);
		void SetLifeTime(float value);
		void SetVelocity(Vector3 value);
		void SetAcceleration(Vector3 value);
		void SetStartColor(Vector4 value);
		void SetEndColor(Vector4 value);
		void SetColorTexture(TextureWrapper^ texture);
		void SetScreenSpaceCollision(bool enabled);

	private:
		IParticleSystem* m_NativeParticleSystem;
	};
}