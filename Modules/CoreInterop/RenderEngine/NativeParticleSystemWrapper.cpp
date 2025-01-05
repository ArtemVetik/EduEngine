#include "NativeParticleSystemWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	NativeParticleSystemWrapper::NativeParticleSystemWrapper()
	{
		m_NativeParticleSystem = CoreSystems::GetInstance()->GetRenderEngine()->CreateParticleSystem();
	}

	NativeParticleSystemWrapper::~NativeParticleSystemWrapper()
	{
		this->!NativeParticleSystemWrapper();
	}

	NativeParticleSystemWrapper::!NativeParticleSystemWrapper()
	{
		if (!m_NativeParticleSystem)
			return;

		CoreSystems::GetInstance()->GetRenderEngine()->RemoveParticleSystem(m_NativeParticleSystem);
		m_NativeParticleSystem = nullptr;
	}

	void NativeParticleSystemWrapper::SetMaxParticles(unsigned int num)
	{
		m_NativeParticleSystem->SetMaxParticles(num);
	}

	void NativeParticleSystemWrapper::SetShapeType(UINT value)
	{
		m_NativeParticleSystem->ShapeType = value;
	}

	void NativeParticleSystemWrapper::SetShapeSize(Vector3 value)
	{
		m_NativeParticleSystem->ShapeSize = { value.X, value.Y, value.Z };
	}

	void NativeParticleSystemWrapper::SetCenterPosition(Vector3 value)
	{
		m_NativeParticleSystem->CenterPos = { value.X, value.Y, value.Z };
	}

	void NativeParticleSystemWrapper::SetEmissionRate(float value)
	{
		m_NativeParticleSystem->EmissionRate = value;
	}

	void NativeParticleSystemWrapper::SetLifeTime(float value)
	{
		m_NativeParticleSystem->LifeTime = value;
	}

	void NativeParticleSystemWrapper::SetVelocity(Vector3 value)
	{
		m_NativeParticleSystem->Velocity = { value.X, value.Y, value.Z };
	}

	void NativeParticleSystemWrapper::SetAcceleration(Vector3 value)
	{
		m_NativeParticleSystem->Acceleration = { value.X, value.Y, value.Z };
	}

	void NativeParticleSystemWrapper::SetStartColor(Vector4 value)
	{
		m_NativeParticleSystem->StartColor = { value.X, value.Y, value.Z, value.W };
	}

	void NativeParticleSystemWrapper::SetEndColor(Vector4 value)
	{
		m_NativeParticleSystem->EndColor = { value.X, value.Y, value.Z, value.W };
	}
}