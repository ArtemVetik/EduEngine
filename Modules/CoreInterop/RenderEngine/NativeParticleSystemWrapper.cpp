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

	void NativeParticleSystemWrapper::SetParticlesData(ParticleData% data)
	{
		NativeStaticParticleData nativeData = {};

		pin_ptr<ParticleData> pinnedStruct = &data;
		std::memcpy(&nativeData, pinnedStruct, sizeof(NativeStaticParticleData));

		m_NativeParticleSystem->SetParticlesData(nativeData);
	}

	void NativeParticleSystemWrapper::SetCenterPosition(Vector3 position)
	{
		m_NativeParticleSystem->SetCenterPos({ position.X, position.Y, position.Z });
	}

	void NativeParticleSystemWrapper::SetColorTexture(TextureWrapper^ texture)
	{
		if (texture == nullptr)
			m_NativeParticleSystem->SetColorTexture(nullptr);
		else
			m_NativeParticleSystem->SetColorTexture(texture->GetNative());
	}
}