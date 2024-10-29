#include "pch.h"
#include "SwapChain.h"
#include "QueryInterface.h"

namespace EduEngine
{
	SwapChain::SwapChain(RenderDeviceD3D12* pDevice)
	{
		m_Device = pDevice;
		m_SwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC desc;
		desc.BufferDesc.Width = Window::GetInstance()->GetClientWidth();
		desc.BufferDesc.Height = Window::GetInstance()->GetClientHeight();
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = BACK_BUFFER_FORMAT;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = SwapChainBufferCount;
		desc.OutputWindow = Window::GetInstance()->GetMainWindow();
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory));

		mDXGIFactory->CreateSwapChain(
			m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT).GetD3D12CommandQueue(),
			&desc,
			m_SwapChain.GetAddressOf());
	}

	void SwapChain::Resize(UINT width, UINT height)
	{
		for (int i = 0; i < SwapChainBufferCount; ++i)
			m_SwapChainBuffers[i].reset();

		m_SwapChain->ResizeBuffers(
			SwapChainBufferCount,
			width, height,
			BACK_BUFFER_FORMAT,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		mCurrentBackBuffer = 0;

		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));

			m_SwapChainBuffers[i] = std::make_unique<TextureD3D12>(m_Device, backBuffer);
			m_SwapChainBuffers[i]->CreateRTVView(nullptr, true);
			m_SwapChainBuffers[i]->SetName(L"SwapChain");
		}

		D3D12_RESOURCE_DESC dsDesc;
		dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsDesc.Alignment = 0;
		dsDesc.Width = Window::GetInstance()->GetClientWidth();
		dsDesc.Height = Window::GetInstance()->GetClientHeight();
		dsDesc.DepthOrArraySize = 1;
		dsDesc.MipLevels = 1;

		dsDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE dsClear;
		dsClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsClear.DepthStencil.Depth = 1.0f;
		dsClear.DepthStencil.Stencil = 0;

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;

		m_DepthStencilTexture = std::make_unique<TextureD3D12>(m_Device, dsDesc, dsClear);
		m_DepthStencilTexture->CreateDSVView(&dsvDesc, true);

		m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).ResourceBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilTexture->GetD3D12Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	}

	ID3D12Resource* SwapChain::CurrentBackBuffer() const
	{
		return m_SwapChainBuffers[mCurrentBackBuffer]->GetD3D12Resource();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::CurrentBackBufferView() const
	{
		return m_SwapChainBuffers[mCurrentBackBuffer]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)->GetCpuHandle();
	}
}