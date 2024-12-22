#include "MaterialWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	MaterialWrapper::MaterialWrapper()
	{
		m_NativeMaterial = CoreSystems::GetInstance()->GetRenderEngine()->CreateMaterial();
	}

	MaterialWrapper::~MaterialWrapper()
	{
		this->!MaterialWrapper();
	}

	MaterialWrapper::!MaterialWrapper()
	{
		if (!m_NativeMaterial)
			return;

		CoreSystems::GetInstance()->GetRenderEngine()->RemoveMaterial(m_NativeMaterial);
		m_NativeMaterial = nullptr;
	}

	void MaterialWrapper::SetTexture(TextureWrapper^ texture)
	{
		if (texture == nullptr)
			m_NativeMaterial->SetMainTexture(nullptr);
		else
			m_NativeMaterial->SetMainTexture(texture->GetNative());
	}

	Vector4 MaterialWrapper::DiffuseAlbedo::get()
	{
#define ALBEDO m_NativeMaterial->DiffuseAlbedo
		return Vector4(ALBEDO.x, ALBEDO.y, ALBEDO.z, ALBEDO.w);
#undef ALBEDO
	}

	void MaterialWrapper::DiffuseAlbedo::set(Vector4 value)
	{
		m_NativeMaterial->DiffuseAlbedo = { value.X, value.Y, value.Z, value.W };
	}

	Vector3 MaterialWrapper::FresnelR0::get()
	{
#define FRESNEL m_NativeMaterial->FresnelR0
		return Vector3(FRESNEL.x, FRESNEL.y, FRESNEL.z);
#undef FRESNEL
	}

	void MaterialWrapper::FresnelR0::set(Vector3 value)
	{
		m_NativeMaterial->FresnelR0 = { value.X, value.Y, value.Z };
	}

	float MaterialWrapper::Roughness::get()
	{
		return m_NativeMaterial->Roughness;
	}

	void MaterialWrapper::Roughness::set(float value)
	{
		m_NativeMaterial->Roughness = value;
	}
}