#include "pch.h"
#include "RenderEngine.h"

namespace EduEngine
{
	RenderEngine* RenderEngine::Instance = nullptr;

	RenderEngine* RenderEngine::GetInstance()
	{
		return Instance;
	}

	RenderEngine::RenderEngine()
	{
		assert(Instance == nullptr);
		Instance = this;

		Window::GetInstance()->SetResizeEvent([&]() { this->Resize(); });
	}

	RenderEngine::~RenderEngine()
	{
		if (m_Device != nullptr)
			m_Device->FlushQueues();
	}

	bool RenderEngine::StartUp()
	{
#if defined(DEBUG) || defined(_DEBUG) 
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
#endif

		Microsoft::WRL::ComPtr<ID3D12Device> device;
		HRESULT hardwareResult = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(device.GetAddressOf()));

		if (FAILED(hardwareResult))
			throw;

		m_Device = std::make_unique<RenderDeviceD3D12>(device);

#if defined(DEGUG) || defined(_DEBUG)
		QueryInterface::GetInstance().Initialize(m_Device->GetD3D12Device());
#endif

		m_SwapChain = std::make_unique<SwapChain>(m_Device.get());
		Resize();

		return true;
	}

	void RenderEngine::Update()
	{
		InputManager::GetInstance().Update();
	}

	void RenderEngine::Draw()
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		D3D12_VIEWPORT ScreenViewPort;
		D3D12_RECT ScissorRect;
		ScreenViewPort.TopLeftX = 0;
		ScreenViewPort.TopLeftY = 0;
		ScreenViewPort.Width = static_cast<float>(Window::GetInstance()->GetClientWidth());
		ScreenViewPort.Height = static_cast<float>(Window::GetInstance()->GetClientHeight());
		ScreenViewPort.MinDepth = 0.0f;
		ScreenViewPort.MaxDepth = 1.0f;

		ScissorRect = { 0, 0, Window::GetInstance()->GetClientWidth(), Window::GetInstance()->GetClientHeight() };

		commandContext.Reset();
		commandContext.SetViewports(&ScreenViewPort, 1);
		commandContext.SetScissorRects(&ScissorRect, 1);
		commandContext.SetRenderTargets(1, &(m_SwapChain->CurrentBackBufferView()), true, &(m_SwapChain->DepthStencilView()));

		commandContext.FlushResourceBarriers();
		commandQueue.CloseAndExecuteCommandContext(&commandContext);

		m_SwapChain->Present();
		m_Device->FinishFrame();
	}

	void RenderEngine::Resize()
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();

		m_SwapChain->Resize(Window::GetInstance()->GetClientWidth(), Window::GetInstance()->GetClientHeight());

		commandContext.FlushResourceBarriers();
		commandQueue.CloseAndExecuteCommandContext(&commandContext);
	}
}