#include "pch.h"
#include "RenderEngine.h"
#include "../Graphics/DynamicUploadBuffer.h"
#include "RenderEngineInternal.h"

namespace EduEngine
{
	RenderEngine* RenderEngine::m_Instance = nullptr;

	RenderEngine* RenderEngine::GetInstance()
	{
		return m_Instance;
	}

	Camera* RenderEngine::CreateCamera()
	{
		auto camera = std::make_shared<Camera>(m_SwapChain->GetWidth(), m_SwapChain->GetHeight());
		m_Cameras.emplace_back(camera);
		return camera.get();
	}

	void RenderEngine::RemoveCamera(Camera* camera)
	{
		m_Cameras.erase(std::remove_if(m_Cameras.begin(), m_Cameras.end(),
			[&camera](const std::shared_ptr<Camera>& ptr) {
				return ptr.get() == camera;
			}),
			m_Cameras.end());
	}

	RenderEngine::RenderEngine() :
		m_Viewport{},
		m_ScissorRect{}
	{
		assert(m_Instance == nullptr);
		m_Instance = this;
	}

	RenderEngine::~RenderEngine()
	{
		m_RenderObjects.clear();

		if (m_Device != nullptr)
			m_Device->FlushQueues();
	}

	bool RenderEngine::StartUp(const RuntimeWindow& mainWindow)
	{
		auto device = RenderEngineInternal::GetInstance().GetDevice();

		m_Device = std::make_unique<RenderDeviceD3D12>(device);

#if defined(DEGUG) || defined(_DEBUG)
		QueryInterface::GetInstance().Initialize(device);
#endif

		m_SwapChain = std::make_unique<SwapChain>(m_Device.get(),
			mainWindow.GetClientWidth(), mainWindow.GetClientHeight(), mainWindow.GetMainWindow());

		Resize(mainWindow.GetClientWidth(), mainWindow.GetClientHeight());

		m_OpaquePass = std::make_unique<OpaquePass>(m_Device.get());
		m_DebugRenderer = std::make_shared<DebugRendererSystem>(m_Device.get());

		m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).Reset();

		return true;
	}

	IRenderObject* RenderEngine::AddObject(NativeMeshData meshData)
	{
		auto renderObject = std::make_shared<RenderObject>();
		renderObject->VertexBuffer = std::make_shared<VertexBufferD3D12>(m_Device.get(), meshData.Vertices.data(),
			sizeof(NativeVertex), (UINT)meshData.Vertices.size());
		renderObject->IndexBuffer = std::make_shared<IndexBufferD3D12>(m_Device.get(), meshData.GetIndices16().data(),
			sizeof(uint16), (UINT)meshData.GetIndices16().size(), DXGI_FORMAT_R16_UINT);

		m_RenderObjects.emplace_back(renderObject);
		return renderObject.get();
	}

	void RenderEngine::RemoveObject(IRenderObject* object)
	{
		m_RenderObjects.erase(std::remove_if(m_RenderObjects.begin(), m_RenderObjects.end(),
			[&object](const std::shared_ptr<RenderObject>& ptr) {
				return ptr.get() == object;
			}),
			m_RenderObjects.end());
	}

	void RenderEngine::Draw()
	{
		if (m_PendingResize != EmptyResize)
		{
			RuntimeWindow::GetInstance()->SetPosition(m_PendingResize.x, m_PendingResize.y, m_PendingResize.width, m_PendingResize.height);
			Resize(m_PendingResize.width, m_PendingResize.height);
			m_PendingResize = EmptyResize;
		}

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();
		commandContext.SetViewports(&m_Viewport, 1);
		commandContext.SetScissorRects(&m_ScissorRect, 1);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		commandContext.FlushResourceBarriers();

		commandContext.GetCmdList()->ClearRenderTargetView(m_SwapChain->CurrentBackBufferView(), DirectX::Colors::Black, 0, nullptr);
		commandContext.GetCmdList()->ClearDepthStencilView(m_SwapChain->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		commandContext.SetRenderTargets(1, &(m_SwapChain->CurrentBackBufferView()), true, &(m_SwapChain->DepthStencilView()));

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_Device->GetD3D12DescriptorHeap() };
		commandContext.GetCmdList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		for (auto& camera : m_Cameras)
		{
			commandContext.GetCmdList()->SetPipelineState(m_OpaquePass->GetD3D12PipelineState());
			commandContext.GetCmdList()->SetGraphicsRootSignature(m_OpaquePass->GetD3D12RootSignature());

			PassConstants passConstants;
			XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(camera->GetViewProjMatrix()));

			DynamicUploadBuffer passUploadBuffer(m_Device.get(), QueueID::Direct);
			passUploadBuffer.LoadData(passConstants);

			for (int i = 0; i < m_RenderObjects.size(); i++)
			{
				auto renderObject = m_RenderObjects[i];

				ObjectConstants objConstants;
				objConstants.World = renderObject->WorldMatrix.Transpose();

				DynamicUploadBuffer uploadBuffer(m_Device.get(), QueueID::Direct);
				uploadBuffer.LoadData(objConstants);
				uploadBuffer.CreateCBV();

				commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(renderObject->VertexBuffer->GetView()));
				commandContext.GetCmdList()->IASetIndexBuffer(&(renderObject->IndexBuffer->GetView()));
				commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(0, passUploadBuffer.GetAllocation().GPUAddress);
				commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(1, uploadBuffer.GetCBVDescriptorGPUHandle());

				commandContext.GetCmdList()->DrawIndexedInstanced(renderObject->IndexBuffer->GetLength(), 1, 0, 0, 0);
			}

			m_DebugRenderer->Render(camera->GetViewProjMatrix(), camera->GetPosition());
		}

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		commandContext.FlushResourceBarriers();
		commandQueue.CloseAndExecuteCommandContext(&commandContext);

		commandContext.Reset();

		m_SwapChain->Present();
		m_Device->FinishFrame();
	}

	void RenderEngine::MoveAndResize(UINT x, UINT y, UINT w, UINT h)
	{
		UINT lx, ly, lw, lh;
		RuntimeWindow::GetInstance()->GetPosition(lx, ly, lw, lh);

		if (lx != x || ly != y || lw != w || lh != h)
			m_PendingResize = {(long)x, (long)y, (long)w, (long)h};
	}

	void RenderEngine::Resize(UINT w, UINT h)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();

		m_SwapChain->Resize(w, h);

		commandContext.FlushResourceBarriers();
		commandQueue.CloseAndExecuteCommandContext(&commandContext);

		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = w;
		m_Viewport.Height = h;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScissorRect = { 0, 0, (int)w, (int)h };

		for (auto camera : m_Cameras)
			camera->SetProjectionMatrix(w, h);
	}

	DirectX::SimpleMath::Vector2 RenderEngine::GetScreenSize() const
	{
		return DirectX::SimpleMath::Vector2(m_SwapChain->GetWidth(), m_SwapChain->GetHeight());
	}
}