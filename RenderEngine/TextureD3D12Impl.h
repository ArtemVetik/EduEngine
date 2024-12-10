#pragma once
#include "IRenderEngine.h"
#include "../Graphics/TextureD3D12.h"

namespace EduEngine
{
	class TextureD3D12Impl : public ITexture
	{
	public:
		TextureD3D12Impl(RenderDeviceD3D12* device, const wchar_t* filePath);
		~TextureD3D12Impl();

		void UpdateFilePath(const wchar_t* filePath) override;
		void* GetGPUPtr() override;

		void Load();
		void Free();

		const wchar_t* GetPath() { return m_FilePath; }

	private:
		RenderDeviceD3D12* m_Device;
		const wchar_t* m_FilePath;
		std::shared_ptr<TextureD3D12> m_Texture;
		int m_RefCount;
	};
}