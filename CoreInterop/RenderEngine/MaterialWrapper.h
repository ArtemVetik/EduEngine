#pragma once
#include "../../RenderEngine/IRenderEngine.h"
#include "TextureWrapper.h"

namespace EduEngine
{
	ref class MaterialWrapper
	{
	public:
		MaterialWrapper();
		~MaterialWrapper();
		!MaterialWrapper();

		void SetTexture(TextureWrapper^ texture);

		IMaterial* GetNative() { return m_NativeMaterial; }

	private:
		IMaterial* m_NativeMaterial;
	};
}