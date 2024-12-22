#pragma once
#include "pch.h"
#include "RenderDeviceD3D12.h"
#include "TextureD3D12.h"
#include <dcomp.h>

#pragma comment(lib,"dcomp.lib")

namespace EduEngine
{
	class GRAPHICS_API SwapChain
	{
	public:
		SwapChain(RenderDeviceD3D12* pDevice, UINT width, UINT height, HWND window, bool transparent = false);

		void Resize(UINT width, UINT height);
		void Present();

		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
	private:
		static const int SwapChainBufferCount = 3;

		EduEngine::DescriptorHeapAllocation mAllocation;
		Microsoft::WRL::ComPtr<IDXGIFactory4> mDXGIFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;

		Microsoft::WRL::ComPtr<IDCompositionDevice> m_DcompDevice;
		Microsoft::WRL::ComPtr<IDCompositionTarget> m_DcompTarget;
		Microsoft::WRL::ComPtr<IDCompositionVisual> m_DcompVisual;

		std::unique_ptr<TextureD3D12> m_SwapChainBuffers[SwapChainBufferCount];
		std::unique_ptr<TextureD3D12> m_DepthStencilTexture;

		RenderDeviceD3D12* m_Device;
		int m_CurrentBackBuffer;
		int m_Width;
		int m_Height;
		bool m_Transparent;
	};
}