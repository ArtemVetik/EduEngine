#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "pch.h"
#include "Window.h"
#include "QueryInterface.h"
#include "RenderDeviceD3D12.h"
#include "SwapChain.h"
#include "TextureD3D12.h"
#include "../Graphics.Heaps/DynamicUploadHeap.h"
#include "BufferD3D12.h"
#include "RenderPasses.h"
#include "RenderObject.h"
#include "Camera.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace EduEngine
{
	class GRAPHICS_API RenderEngine
	{
	public:
		RenderEngine();

		RenderEngine(const RenderEngine& rhs) = delete;
		RenderEngine& operator=(const RenderEngine& rhs) = delete;
		~RenderEngine();

		bool StartUp(const Window& mainWindow, RenderDeviceD3D12** ppDeviceOut);

		void Update();
		void Draw();
		void Resize(int width, int height);

		void SetRenderObjects(RenderObject* pBuff, UINT count);
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

		RenderObject* m_RenderObjectsData;
		UINT m_RenderObjectsCount;
	};
}