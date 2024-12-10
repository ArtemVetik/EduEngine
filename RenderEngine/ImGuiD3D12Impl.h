#pragma once
#include "framework.h"
#include "RenderPasses.h"
#include "../Graphics/TextureD3D12.h"
#include "imgui/imgui.h"

namespace EduEngine
{
	class RENDERENGINE_API ImGuiD3D12Impl
	{
	public:
		ImGuiD3D12Impl(RenderDeviceD3D12* device,
					   ImGuiPass*		  renderPass,
					   void*			  pixels,
					   int				  width,
					   int				  height,
					   int				  bytesPerPixel);

		void* GetFontTexturePtr() const { return reinterpret_cast<void*>(m_FontTexture->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetGpuHandle().ptr); }

		void Update(ImDrawData* drawData);
		void Draw(float displayWidth, float displayHeight);

	private:
		RenderDeviceD3D12* m_Device;
		std::shared_ptr<TextureD3D12> m_FontTexture;

		ImGuiPass* m_RenderPass = nullptr;
		ImDrawData* m_DrawData = nullptr;
	};
}