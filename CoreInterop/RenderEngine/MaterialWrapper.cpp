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
}