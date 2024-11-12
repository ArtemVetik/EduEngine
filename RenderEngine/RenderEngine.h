#pragma once

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
#include "Window.h"
#include "RenderObject.h"

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

		bool StartUp(const Window& mainWindow);

		IRenderObject* AddObject(NativeMeshData meshData) override;
		void RemoveObject(IRenderObject* object) override;
		void SetCamera(Camera* camera) override;
		void Draw() override;

		void Resize(int width, int height);

		IDebugRendererSystem* GetDebugRender() const override { return m_DebugRenderer.get(); }

		static RenderEngine* GetInstance();
	private:
		static RenderEngine* Instance;

		std::vector<std::shared_ptr<RenderObject>> m_RenderObjects;

		std::unique_ptr<RenderDeviceD3D12> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;

		std::unique_ptr<OpaquePass> m_OpaquePass;
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		Camera* m_Camera;
		std::shared_ptr<DebugRendererSystem> m_DebugRenderer;
	};
}