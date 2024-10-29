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
		/*if (m_Device.GetD3D12Device() != nullptr)
			m_Device.Flush();*/
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

	void RenderEngine::Draw(const Timer& timer)
	{

	}

	void RenderEngine::Resize()
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();

		m_SwapChain->Resize(Window::GetInstance()->GetClientWidth(), Window::GetInstance()->GetClientHeight());

		commandQueue.CloseAndExecuteCommandContext(&commandContext);
	}
}