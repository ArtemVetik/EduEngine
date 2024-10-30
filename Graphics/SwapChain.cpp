#include "pch.h"
#include "SwapChain.h"
#include "QueryInterface.h"

namespace EduEngine
{
	SwapChain::SwapChain(RenderDeviceD3D12* pDevice) :
		m_Device(pDevice),
		m_CurrentBackBuffer(0)
	{
		m_SwapChain.Reset();

		UINT createFactoryFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&mDXGIFactory));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = Window::GetInstance()->GetClientWidth();
		swapChainDesc.Height = Window::GetInstance()->GetClientHeight();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = SwapChainBufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		mDXGIFactory->CreateSwapChainForHwnd(
			m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT).GetD3D12CommandQueue(),
			Window::GetInstance()->GetMainWindow(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1);

		// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
		// will be handled manually.
		ThrowIfFailed(mDXGIFactory->MakeWindowAssociation(Window::GetInstance()->GetMainWindow(), DXGI_MWA_NO_ALT_ENTER));

		ThrowIfFailed(swapChain1.As(&m_SwapChain));

		m_SwapChain->SetMaximumFrameLatency(SwapChainBufferCount);
	}

	void SwapChain::Resize(UINT width, UINT height)
	{
		for (int i = 0; i < SwapChainBufferCount; ++i)
			m_SwapChainBuffers[i].reset();

		m_DepthStencilTexture.reset();
		
		// m_DepthStencilTexture must reset before m_SwapChain->ResizeBuffers
		// therefore, flush and release resources
		m_Device->FlushQueues();
		m_Device->FinishFrame();

		m_SwapChain->ResizeBuffers(
			SwapChainBufferCount,
			width, height,
			BACK_BUFFER_FORMAT,
			DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);

		m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();

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
		m_DepthStencilTexture->SetName(L"MainDepthStencil");
		m_DepthStencilTexture->CreateDSVView(&dsvDesc, true);

		m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).ResourceBarrier(
			CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilTexture->GetD3D12Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	}

	void SwapChain::Present()
	{
		HANDLE m_frameLatencyWaitableObject = m_SwapChain->GetFrameLatencyWaitableObject();
		DWORD result = WaitForSingleObjectEx(
			m_frameLatencyWaitableObject,
			500, // 0.5 second timeout (shouldn't ever occur)
			true
		);

		m_SwapChain->Present(0, 0);
		m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();
	}

	ID3D12Resource* SwapChain::CurrentBackBuffer() const
	{
		return m_SwapChainBuffers[m_CurrentBackBuffer]->GetD3D12Resource();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::CurrentBackBufferView() const
	{
		return m_SwapChainBuffers[m_CurrentBackBuffer]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)->GetCpuHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::DepthStencilView() const
	{
		return m_DepthStencilTexture->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)->GetCpuHandle();
	}
}