#pragma once
#include "../Graphics/framework.h"
#include "../Graphics/QueryInterface.h"
#include "../Graphics/RenderDeviceD3D12.h"
#include "../Graphics/SwapChain.h"
#include "../Graphics/TextureD3D12.h"
#include "../Graphics/BufferD3D12.h"
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
#include "MaterialD3D12Impl.h"
#include "DeferredRendering.h"
#include "CSMRendering.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace EduEngine
{
	class RenderResourcesInfo;

	class RenderEngine : public IRenderEngine
	{
	public:
		RenderEngine();

		RenderEngine(const RenderEngine& rhs) = delete;
		RenderEngine& operator=(const RenderEngine& rhs) = delete;
		~RenderEngine();

		bool StartUp(const RuntimeWindow& mainWindow);

		IRenderObject* AddObject() override;
		void RemoveObject(IRenderObject* object) override;
		IMesh* CreateMesh(const char* filePath) override;
		void RemoveMesh(IMesh* mesh) override;
		ITexture* CreateTexture(const wchar_t* filePath) override;
		void RemoveTexture(ITexture* texture) override;
		IMaterial* CreateMaterial() override;
		void RemoveMaterial(IMaterial* material) override;
		Camera* CreateCamera() override;
		void RemoveCamera(Camera* camera) override;
		Light* CreateLight() override;
		void RemoveLight(Light* light) override;

		void BeginDraw() override;
		void Draw(Camera* camera) override;
		void EndDraw() override;
		void MoveAndResize(UINT x, UINT y, UINT w, UINT h) override;

		void Resize(UINT w, UINT h);

		DirectX::SimpleMath::Vector2 GetScreenSize() const override;

		IDebugRendererSystem* GetDebugRender() const override { return m_DebugRenderer.get(); }

		static RenderEngine* GetInstance();

		friend class RenderResourcesInfo;
	private:
		static RenderEngine* m_Instance;

		std::unique_ptr<RenderDeviceD3D12> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;

		std::vector<std::shared_ptr<RenderObject>> m_RenderObjects;
		std::vector<std::shared_ptr<SharedMeshD3D12Impl>> m_SharedMeshes;
		std::vector<std::shared_ptr<TextureD3D12Impl>> m_Textures;
		std::vector<std::shared_ptr<MaterialD3D12Impl>> m_Materials;
		std::vector<std::shared_ptr<Camera>> m_Cameras;
		std::vector<std::shared_ptr<Light>> m_Lights;

		std::unique_ptr<OpaquePass> m_OpaquePass;
		std::unique_ptr<DeferredRendering> m_DeferredRendering;
		std::unique_ptr<CSMRendering> m_CSMRendering;

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		std::shared_ptr<DebugRendererSystem> m_DebugRenderer;

		static constexpr XMFLOAT2 CSMSizes[4] =
		{
			{ 2048, 2048 },
			{ 1024, 1024 },
			{ 512,	512	 },
			{ 256,	256	 },
		};
		static constexpr float CSMSplits[4] = { 0.25f, 0.50f, 0.75f, 1.0f };
		static constexpr float CSMDistance = 150.0f;

		static constexpr DirectX::SimpleMath::Rectangle EmptyResize = {-1, -1, -1, -1};
		DirectX::SimpleMath::Rectangle m_PendingResize = EmptyResize;
	};
}