#include "pch.h"
#include "MaterialD3D12Impl.h"

namespace EduEngine
{
	MaterialD3D12Impl::MaterialD3D12Impl() :
		m_MainTexture(nullptr),
		m_RefCount(0)
	{
	}

	MaterialD3D12Impl::~MaterialD3D12Impl()
	{
		if (m_MainTexture)
			m_MainTexture->Free();
	}

	void MaterialD3D12Impl::SetMainTexture(ITexture* texture)
	{
		if (m_MainTexture && m_RefCount > 0)
			m_MainTexture->Free();

		m_MainTexture = dynamic_cast<TextureD3D12Impl*>(texture);

		if (m_MainTexture && m_RefCount > 0)
			m_MainTexture->Load();
	}

	ITexture* MaterialD3D12Impl::GetMainTexture()
	{
		return m_MainTexture;
	}

	void MaterialD3D12Impl::Load()
	{
		if (m_RefCount > 0)
		{
			m_RefCount++;
			return;
		}

		if (m_MainTexture)
			m_MainTexture->Load();

		m_RefCount = 1;
	}

	void MaterialD3D12Impl::Free()
	{
		if (m_RefCount <= 0)
			return;

		m_RefCount--;

		if (m_RefCount == 0)
		{
			if (m_MainTexture)
				m_MainTexture->Free();
		}
	}
}