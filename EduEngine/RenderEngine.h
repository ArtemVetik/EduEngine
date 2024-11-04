#pragma once

#include "../Graphics/framework.h"
#include "../Graphics/QueryInterface.h"
#include "../Graphics/RenderDeviceD3D12.h"
#include "../Graphics/SwapChain.h"
#include "../Graphics/TextureD3D12.h"
#include "../Graphics/BufferD3D12.h"
#include "../Graphics/RenderObject.h"
#include "../Graphics/Camera.h"
#include "../GameplayCore/Scene.h"
#include "RenderPasses.h"
#include "Window.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace EduEngine
{
	class RenderEngine
	{
	public:
		RenderEngine();

		RenderEngine(const RenderEngine& rhs) = delete;
		RenderEngine& operator=(const RenderEngine& rhs) = delete;
		~RenderEngine();

		bool StartUp(const Window& mainWindow, RenderDeviceD3D12** ppDeviceOut);

		void Draw();
		void Resize(int width, int height);

		void SetScene(const Scene* scene);
		void SetCamera(Camera* pCamera);

		static RenderEngine* GetInstance();
	private:
		static RenderEngine* Instance;

		std::unique_ptr<RenderDeviceD3D12> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;

		std::unique_ptr<OpaquePass> m_OpaquePass;
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		Camera* m_Camera;

		const Scene* m_Scene;
	};
}