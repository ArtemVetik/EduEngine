#include "pch.h"
#include "SwapChain.h"

namespace EduEngine
{
	SwapChain::SwapChain(RenderDeviceD3D12* pDevice, UINT width, UINT height, HWND window, bool transparent /* = false */) :
		m_Device(pDevice),
		m_CurrentBackBuffer(0),
		m_Transparent(transparent)
	{
		m_SwapChain.Reset();

		UINT createFactoryFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&mDXGIFactory));

		m_Width = width;
		m_Height = height;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = SwapChainBufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		if (transparent)
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		else
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;

		if (transparent)
		{
			mDXGIFactory->CreateSwapChainForComposition(
				m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT).GetD3D12CommandQueue(),
				&swapChainDesc,
				nullptr,
				&swapChain1
			);

			//------------------------------------------------------------------
			// Set up DirectComposition
			//------------------------------------------------------------------

			// Create the DirectComposition device
			HRESULT hr = DCompositionCreateDevice(
				nullptr,
				IID_PPV_ARGS(m_DcompDevice.ReleaseAndGetAddressOf()));
			if (FAILED(hr)) throw;

			// Create a DirectComposition target associated with the window (pass in hWnd here)
			hr = (m_DcompDevice->CreateTargetForHwnd(
				window,
				true,
				m_DcompTarget.ReleaseAndGetAddressOf()));
			if (FAILED(hr)) throw;

			// Create a DirectComposition "visual"
			hr = (m_DcompDevice->CreateVisual(m_DcompVisual.ReleaseAndGetAddressOf()));
			if (FAILED(hr)) throw;

			// Associate the visual with the swap chain
			hr = (m_DcompVisual->SetContent(swapChain1.Get()));
			if (FAILED(hr)) throw;

			// Set the visual as the root of the DirectComposition target's composition tree
			hr = (m_DcompTarget->SetRoot(m_DcompVisual.Get()));
			if (FAILED(hr)) throw;

			hr = (m_DcompDevice->Commit());
			if (FAILED(hr)) throw;
		}
		else
		{
			mDXGIFactory->CreateSwapChainForHwnd(
				m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT).GetD3D12CommandQueue(),
				window,
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain1);
		}

		// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
		// will be handled manually.
		HRESULT hr = mDXGIFactory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);
		THROW_IF_FAILED(hr, L"Failed to make window association");

		hr = swapChain1.As(&m_SwapChain);
		THROW_IF_FAILED(hr, L"Failed to cast swapchain");

		m_SwapChain->SetMaximumFrameLatency(SwapChainBufferCount);
	}

	void SwapChain::Resize(UINT width, UINT height)
	{
		m_Width = width;
		m_Height = height;

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

			m_SwapChainBuffers[i] = std::make_unique<TextureD3D12>(m_Device, backBuffer, QueueID::Direct);
			m_SwapChainBuffers[i]->CreateRTVView(nullptr, true);
			m_SwapChainBuffers[i]->SetName(L"SwapChain");
		}

		D3D12_RESOURCE_DESC dsDesc;
		dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsDesc.Alignment = 0;
		dsDesc.Width = width;
		dsDesc.Height = height;
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

		m_DepthStencilTexture = std::make_unique<TextureD3D12>(m_Device, dsDesc, &dsClear, QueueID::Direct);
		m_DepthStencilTexture->SetName(L"MainDepthStencil");
		m_DepthStencilTexture->CreateDSVView(&dsvDesc, true);

		D3D12_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
		depthSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		depthSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		depthSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		depthSrvDesc.Texture2D.MostDetailedMip = 0;
		depthSrvDesc.Texture2D.MipLevels = 1;
		depthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		depthSrvDesc.Texture2D.PlaneSlice = 0;
		m_DepthStencilTexture->CreateSRVView(&depthSrvDesc, false);

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

		if (m_Transparent)
			m_SwapChain->Present(1, 0);
		else
			m_SwapChain->Present(0, 0);

		m_CurrentBackBuffer = m_SwapChain->GetCurrentBackBufferIndex();
	}

	ID3D12Resource* SwapChain::CurrentBackBuffer() const
	{
		return m_SwapChainBuffers[m_CurrentBackBuffer]->GetD3D12Resource();
	}

	ID3D12Resource* SwapChain::GetDepthStencilBuffer() const
	{
		return m_DepthStencilTexture->GetD3D12Resource();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::CurrentBackBufferView() const
	{
		return m_SwapChainBuffers[m_CurrentBackBuffer]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)->GetCpuHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::DepthStencilView() const
	{
		return m_DepthStencilTexture->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)->GetCpuHandle();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE SwapChain::DepthStencilSRVView() const
	{
		return m_DepthStencilTexture->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetGpuHandle();
	}
}