#pragma once
#include "../../RenderEngine/IRenderEngine.h"
#include "TextureWrapper.h"

namespace EduEngine
{
	using namespace System::Numerics;

	ref class MaterialWrapper
	{
	public:
		MaterialWrapper();
		~MaterialWrapper();
		!MaterialWrapper();

		property Vector4 DiffuseAlbedo
		{ 
			Vector4 get();
			void set(Vector4 value);
		}

		property Vector3 FresnelR0
		{
			Vector3 get();
			void set(Vector3 value);
		}

		property float Roughness
		{
			float get();
			void set(float value);
		}

		void SetTexture(TextureWrapper^ texture);

		IMaterial* GetNative() { return m_NativeMaterial; }

	private:
		IMaterial* m_NativeMaterial;
	};
}