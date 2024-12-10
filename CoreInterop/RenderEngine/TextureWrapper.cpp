#include "TextureWrapper.h"
#include "../CoreSystems.h"
#include "../Utils.h"

namespace EduEngine
{
	TextureWrapper::TextureWrapper(String^ filePath)
	{
		auto cFilePath = (Utils::ToWCharPointer(filePath));
		m_NativeTexture = CoreSystems::GetInstance()->GetRenderEngine()->CreateTexture(cFilePath);
	}

	TextureWrapper::~TextureWrapper()
	{
		this->!TextureWrapper();
	}

	TextureWrapper::!TextureWrapper()
	{
		if (!m_NativeTexture)
			return;

		CoreSystems::GetInstance()->GetRenderEngine()->RemoveTexture(m_NativeTexture);
		m_NativeTexture = nullptr;
	}

	void TextureWrapper::UpdateFilePath(String^ filePath)
	{
		if (!m_NativeTexture)
			return;

		auto cFilePath = (Utils::ToWCharPointer(filePath));
		m_NativeTexture->UpdateFilePath(cFilePath);
	}

	IntPtr TextureWrapper::GetGPUPtr()
	{
		if (!m_NativeTexture)
			return IntPtr::Zero;

		return IntPtr(m_NativeTexture->GetGPUPtr());
	}
}