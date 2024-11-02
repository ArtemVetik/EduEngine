#include "pch.h"
#include "RenderEngine.h"

namespace EduEngine
{
	RenderEngine* RenderEngine::Instance = nullptr;

	RenderEngine* RenderEngine::GetInstance()
	{
		return Instance;
	}

	RenderEngine::RenderEngine() :
		m_RenderObjectsData{ nullptr },
		m_RenderObjectsCount{ 0 },
		m_Viewport{},
		m_ScissorRect{},
		m_Camera{nullptr}
	{
		assert(Instance == nullptr);
		Instance = this;
	}

	RenderEngine::~RenderEngine()
	{
		if (m_Device != nullptr)
			m_Device->FlushQueues();
	}

	bool RenderEngine::StartUp(const Window& mainWindow, RenderDeviceD3D12** ppDeviceOut)
	{
#if defined(DEBUG) || defined(_DEBUG) 
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		THROW_IF_FAILED(hr, L"Failed to get debug interface");
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

		m_SwapChain = std::make_unique<SwapChain>(m_Device.get(), mainWindow);

		Resize(mainWindow.GetClientWidth(), mainWindow.GetClientHeight());

		m_OpaquePass = std::make_unique<OpaquePass>(m_Device.get());

		*ppDeviceOut = m_Device.get();
		return true;
	}

	void RenderEngine::Update()
	{
	}

	void RenderEngine::Draw()
	{
		if (m_Camera == nullptr)
			return;

		PassConstants passConstants;
		XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(m_Camera->GetViewProjMatrix()));

		auto passAlloc = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT).AllocateInDynamicHeap(sizeof(PassConstants));
		memcpy(passAlloc.CPUAddress, &passConstants, sizeof(PassConstants));

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();
		commandContext.SetViewports(&m_Viewport, 1);
		commandContext.SetScissorRects(&m_ScissorRect, 1);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		commandContext.FlushResourceBarriers();

		commandContext.GetCmdList()->ClearRenderTargetView(m_SwapChain->CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		commandContext.GetCmdList()->ClearDepthStencilView(m_SwapChain->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		commandContext.SetRenderTargets(1, &(m_SwapChain->CurrentBackBufferView()), true, &(m_SwapChain->DepthStencilView()));

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_Device->GetD3D12DescriptorHeap() };
		commandContext.GetCmdList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		commandContext.GetCmdList()->SetPipelineState(m_OpaquePass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_OpaquePass->GetD3D12RootSignature());

		for (int i = 0; i < m_RenderObjectsCount; i++)
		{
			auto renderObject = m_RenderObjectsData[i];

			ObjectConstants objConstants;
			objConstants.World = renderObject.World;

			auto objAlloc = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT).AllocateInDynamicHeap(sizeof(ObjectConstants));
			memcpy(objAlloc.CPUAddress, &objConstants, sizeof(ObjectConstants));

			commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(renderObject.VertexBuffer->GetView()));
			commandContext.GetCmdList()->IASetIndexBuffer(&(renderObject.IndexBuffer->GetView()));
			commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(0, passAlloc.GPUAddress);
			commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(1, objAlloc.GPUAddress);

			commandContext.GetCmdList()->DrawIndexedInstanced(renderObject.IndexBuffer->GetLength(), 1, 0, 0, 0);
		}

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		commandContext.FlushResourceBarriers();
		commandQueue.CloseAndExecuteCommandContext(&commandContext);

		m_SwapChain->Present();
		m_Device->FinishFrame();
	}

	void RenderEngine::SetRenderObjects(RenderObject* pBuff, UINT count)
	{
		m_RenderObjectsData = pBuff;
		m_RenderObjectsCount = count;
	}

	void RenderEngine::SetCamera(Camera* pCamera)
	{
		m_Camera = pCamera;
	}

	void RenderEngine::Resize(int width, int height)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();

		m_SwapChain->Resize(width, height);

		commandContext.FlushResourceBarriers();
		commandQueue.CloseAndExecuteCommandContext(&commandContext);

		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = width;
		m_Viewport.Height = height;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScissorRect = { 0, 0, width, height };

		if (m_Camera)
			m_Camera->SetProjectionMatrix(width, height);
	}
}