#pragma once
#include "pch.h"
#include "RenderDeviceD3D12.h"
#include "TextureD3D12.h"
#include "Window.h"

namespace EduEngine
{
	class GRAPHICS_API SwapChain
	{
	public:
		SwapChain(RenderDeviceD3D12* pDevice);

		void Resize(UINT width, UINT height);
		void Present();

		ID3D12Resource* CurrentBackBuffer() const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	private:
		static const int SwapChainBufferCount = 3;

		EduEngine::DescriptorHeapAllocation mAllocation;
		Microsoft::WRL::ComPtr<IDXGIFactory4> mDXGIFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;

		std::unique_ptr<TextureD3D12> m_SwapChainBuffers[SwapChainBufferCount];
		std::unique_ptr<TextureD3D12> m_DepthStencilTexture;

		RenderDeviceD3D12* m_Device;
		int m_CurrentBackBuffer;
	};
}