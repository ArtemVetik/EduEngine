#pragma once
#include "IRenderEngine.h"
#include "TextureD3D12Impl.h"

namespace EduEngine
{
	class MaterialD3D12Impl : public IMaterial
	{
	public:
		MaterialD3D12Impl();
		~MaterialD3D12Impl();

		void SetMainTexture(ITexture* texture) override;
		ITexture* GetMainTexture() override;

		void Load();
		void Free();

	private:
		TextureD3D12Impl* m_MainTexture;

		int m_RefCount;
	};
}