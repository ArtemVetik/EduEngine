#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "pch.h"
#include "InputManager.h"
#include "Timer.h"
#include "Window.h"
#include "QueryInterface.h"
#include "RenderDeviceD3D12.h"
#include "SwapChain.h"
#include "TextureD3D12.h"
#include "../Graphics.Heaps/DynamicUploadHeap.h"

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

		bool StartUp();

		void Update();
		void Draw();

		static RenderEngine* GetInstance();
	private:
		static RenderEngine* Instance;

		std::unique_ptr<RenderDeviceD3D12> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;

		void Resize();
	};
}