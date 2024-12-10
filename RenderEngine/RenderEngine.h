#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Graphics/framework.h"
#include "../Graphics/QueryInterface.h"
#include "../Graphics/RenderDeviceD3D12.h"
#include "../Graphics/SwapChain.h"
#include "../Graphics/TextureD3D12.h"
#include "../Graphics/BufferD3D12.h"
#include "../Graphics/RenderObject.h"
#include "DebugRendererSystem.h"
#include "framework.h"
#include "IRenderEngine.h"
#include "Camera.h"
#include "RenderPasses.h"
#include "RuntimeWindow.h"
#include "RenderObject.h"
#include "ImGuiD3D12Impl.h"
#include "SharedMeshD3D12Impl.h"
#include "TextureD3D12Impl.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace EduEngine
{
	class RenderEngine : public IRenderEngine
	{
	public:
		RenderEngine();

		RenderEngine(const RenderEngine& rhs) = delete;
		RenderEngine& operator=(const RenderEngine& rhs) = delete;
		~RenderEngine();

		bool StartUp(const RuntimeWindow& mainWindow);

		IRenderObject* AddObject(IMesh* mesh) override;
		void RemoveObject(IRenderObject* object) override;
		IMesh* CreateMesh(const char* filePath) override;
		void RemoveMesh(IMesh* mesh) override;
		ITexture* CreateTexture(const wchar_t* filePath) override;
		void RemoveTexture(ITexture* texture) override;
		Camera* CreateCamera() override;
		void RemoveCamera(Camera* camera) override;
		
		void BeginDraw() override;
		void Draw(Camera* camera) override;
		void EndDraw() override;
		void MoveAndResize(UINT x, UINT y, UINT w, UINT h) override;

		void Resize(UINT w, UINT h);

		DirectX::SimpleMath::Vector2 GetScreenSize() const override;

		IDebugRendererSystem* GetDebugRender() const override { return m_DebugRenderer.get(); }

		static RenderEngine* GetInstance();
	private:
		static RenderEngine* m_Instance;

		std::unique_ptr<RenderDeviceD3D12> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;

		std::vector<std::shared_ptr<RenderObject>> m_RenderObjects;
		std::vector<std::shared_ptr<SharedMeshD3D12Impl>> m_SharedMeshes;
		std::vector<std::shared_ptr<TextureD3D12Impl>> m_Textures;
		std::vector<std::shared_ptr<Camera>> m_Cameras;

		std::unique_ptr<OpaquePass> m_OpaquePass;
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		std::shared_ptr<DebugRendererSystem> m_DebugRenderer;
		Assimp::Importer m_AssimpImporter[22];

		static constexpr DirectX::SimpleMath::Rectangle EmptyResize = {-1, -1, -1, -1};
		DirectX::SimpleMath::Rectangle m_PendingResize = EmptyResize;
	};
}