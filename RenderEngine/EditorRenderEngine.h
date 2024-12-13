#pragma once
#include "IRenderEngine.h"
#include "../Graphics/RenderDeviceD3D12.h"
#include "../Graphics/SwapChain.h"
#include "ImGuiD3D12Impl.h"
#include "TextureD3D12Impl.h"
#include "EditorMeshPreviewD3D12.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace EduEngine
{
	class EditorRenderEngine : public IEditorRenderEngine
	{
	public:
		EditorRenderEngine();
		~EditorRenderEngine();

		bool StartUp(const EditorWindow& editorWindow);

		void Resize(UINT width, UINT height);

		void Draw() override;
		void UpdateEditor(ImDrawData* drawData) override;
		void* CreateEditorImGuiUI(void* pixels, int texWidth, int texHeight, int bytesPerPixel) override;
		DirectX::SimpleMath::Vector2 GetScreenSize() const override;

		void* SetPreviewTexture(const wchar_t* filePath) override;
		PreviewMeshInfo SetPreviewMesh(const char* filePath) override;
		void RotatePreviewMesh(DirectX::XMFLOAT3 delta) override;

		static EditorRenderEngine* GetInstance() { return m_Instance; }

	private:
		static EditorRenderEngine* m_Instance;

		std::unique_ptr<RenderDeviceD3D12> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;

		std::shared_ptr<TextureD3D12Impl> m_PreviewTex;
		std::shared_ptr<EditorMeshPreviewD3D12> m_PreviewMesh;

		std::unique_ptr<ImGuiPass> m_ImGuiPass;

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		std::unique_ptr<ImGuiD3D12Impl> m_EditorUI = nullptr;
	};
}