#pragma once
#include "../Graphics/framework.h"
#include "../Graphics/RenderDeviceD3D12.h"
#include "../Graphics/SwapChain.h"
#include "../Graphics/TextureD3D12.h"
#include "../Graphics/BufferD3D12.h"
#include "DebugRendererSystem.h"
#include "framework.h"
#include "IRenderEngine.h"
#include "CameraInternal.h"
#include "RenderPasses.h"
#include "RuntimeWindow.h"
#include "RenderObject.h"
#include "ImGuiD3D12Impl.h"
#include "SharedMeshD3D12Impl.h"
#include "TextureD3D12Impl.h"
#include "MaterialD3D12Impl.h"
#include "ParticleSystemD3D12.h"
#include "DeferredRendering.h"
#include "CSMRendering.h"
#include "SkyboxRendering.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace EduEngine
{
	class RenderResourcesInfo;

	class RenderEngine : public IRenderEngine
	{
	public:
		RenderEngine(const Timer& timer);

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
		IParticleSystem* CreateParticleSystem() override;
		void RemoveParticleSystem(IParticleSystem* particleSystem) override;

		void UpdateUI(ImDrawData* drawData) override;
		void* CreateImGuiUI(void* pixels, int texWidth, int texHeight, int bytesPerPixel) override;

		void BeginDraw() override;
		void Draw(Camera* camera) override;
		void EndDraw() override;
		void MoveAndResize(UINT x, UINT y, UINT w, UINT h) override;
		void PendingResize(UINT w, UINT h);

		DirectX::SimpleMath::Vector2 GetScreenSize() const override;

		IDebugRendererSystem* GetDebugRender() const override { return m_DebugRenderer.get(); }
		RenderSettings* GetRenderSettings() override { return &m_RenderSettings; }

		static RenderEngine* GetInstance();

		friend class RenderResourcesInfo;
	private:
		void Resize(UINT w, UINT h);

		static RenderEngine* m_Instance;

		std::unique_ptr<RenderDeviceD3D12> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;

		std::vector<std::shared_ptr<RenderObject>> m_RenderObjects;
		std::vector<std::shared_ptr<SharedMeshD3D12Impl>> m_SharedMeshes;
		std::vector<std::shared_ptr<TextureD3D12Impl>> m_Textures;
		std::vector<std::shared_ptr<MaterialD3D12Impl>> m_Materials;
		std::vector<std::shared_ptr<CameraInternal>> m_Cameras;
		std::vector<std::shared_ptr<Light>> m_Lights;
		std::vector<std::shared_ptr<ParticleSystemD3D12>> m_ParticleSystems;

		std::unique_ptr<DeferredRendering> m_DeferredRendering;
		std::unique_ptr<CSMRendering> m_CSMRendering;
		std::unique_ptr<SkyboxRendering> m_SkyBoxRendering;
		std::shared_ptr<DebugRendererSystem> m_DebugRenderer;
		RenderSettings m_RenderSettings;

		std::unique_ptr<ImGuiD3D12Impl> m_OverlayUI;

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		const Timer& m_Timer;

		static constexpr XMFLOAT2 CSMSizes[4] =
		{
			{ 2048, 2048 },
			{ 1024, 1024 },
			{ 512,	512	 },
			{ 256,	256	 },
		};
		static constexpr float CSMSplits[4] = { 0.25f, 0.50f, 0.75f, 1.0f };

		static constexpr DirectX::SimpleMath::Rectangle EmptyResize = { -1, -1, -1, -1 };
		DirectX::SimpleMath::Rectangle m_PendingResize = EmptyResize;
	};
}