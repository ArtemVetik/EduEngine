#pragma once
#include "../../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	using namespace System;

	ref class TextureWrapper
	{
	public:
		TextureWrapper(String^ filePath);
		~TextureWrapper();
		!TextureWrapper();

		void UpdateFilePath(String^ filePath);

		IntPtr GetGPUPtr();
		ITexture* GetNative() { return m_NativeTexture; }

	private:
		ITexture* m_NativeTexture;
	};
}