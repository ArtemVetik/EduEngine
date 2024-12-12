#include "pch.h"
#include "../Graphics/DynamicUploadBuffer.h"
#include "RenderEngine.h"
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

		D3D12_SHADER_RESOURCE_VIEW_DESC nullSrvDesc = {};
		nullSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		nullSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		nullSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

		auto gpuAlloc = m_Device->AllocateGPUDescriptor(QueueID::Direct, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		m_Device->GetD3D12Device()->CreateShaderResourceView(nullptr, &nullSrvDesc, gpuAlloc.GetCpuHandle());
		m_NullTex = std::make_unique<TextureHeapView>(std::move(gpuAlloc), false);

		return true;
	}

	IRenderObject* RenderEngine::AddObject()
	{
		auto renderObject = std::make_shared<RenderObject>();
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

	IMesh* RenderEngine::CreateMesh(const char* filePath)
	{
		auto mesh = std::make_shared<SharedMeshD3D12Impl>(m_Device.get(), filePath);
		m_SharedMeshes.emplace_back(mesh);
		return mesh.get();
	}

	void RenderEngine::RemoveMesh(IMesh* mesh)
	{
		SharedMeshD3D12Impl* meshD3D12 = dynamic_cast<SharedMeshD3D12Impl*>(mesh);

		for (auto& renderObject : m_RenderObjects)
			if (renderObject->GetMesh() == meshD3D12)
				renderObject->SetMesh(nullptr);

		m_SharedMeshes.erase(std::remove_if(m_SharedMeshes.begin(), m_SharedMeshes.end(),
			[&mesh](const std::shared_ptr<IMesh>& ptr) {
				return ptr.get() == mesh;
			}),
			m_SharedMeshes.end());
	}

	ITexture* RenderEngine::CreateTexture(const wchar_t* filePath)
	{
		auto texture = std::make_shared<TextureD3D12Impl>(m_Device.get(), filePath);
		m_Textures.emplace_back(texture);
		return texture.get();
	}

	void RenderEngine::RemoveTexture(ITexture* texture)
	{
		for (auto& material : m_Materials)
			if (material->GetMainTexture() == texture)
				material->SetMainTexture(nullptr);

		m_Textures.erase(std::remove_if(m_Textures.begin(), m_Textures.end(),
			[&texture](const std::shared_ptr<ITexture>& ptr) {
				return ptr.get() == texture;
			}),
			m_Textures.end());
	}

	IMaterial* RenderEngine::CreateMaterial()
	{
		auto material = std::make_shared<MaterialD3D12Impl>();
		m_Materials.emplace_back(material);
		return material.get();
	}

	void RenderEngine::RemoveMaterial(IMaterial* material)
	{
		for (auto& renderObject : m_RenderObjects)
			if (renderObject->GetMaterial() == material)
				renderObject->SetMaterial(nullptr);

		m_Materials.erase(std::remove_if(m_Materials.begin(), m_Materials.end(),
			[&material](const std::shared_ptr<IMaterial>& ptr) {
				return ptr.get() == material;
			}),
			m_Materials.end());
	}

	void RenderEngine::BeginDraw()
	{
		if (m_PendingResize != EmptyResize)
		{
			RuntimeWindow::GetInstance()->SetPosition(m_PendingResize.x, m_PendingResize.y, m_PendingResize.width, m_PendingResize.height);
			Resize(m_PendingResize.width, m_PendingResize.height);
			m_PendingResize = EmptyResize;
		}

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

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
	}

	void RenderEngine::Draw(Camera* camera)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.GetCmdList()->SetPipelineState(m_OpaquePass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_OpaquePass->GetD3D12RootSignature());

		PassConstants passConstants;
		XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(camera->GetViewProjMatrix()));

		DynamicUploadBuffer passUploadBuffer(m_Device.get(), QueueID::Direct);
		passUploadBuffer.LoadData(passConstants);

		for (int i = 0; i < m_RenderObjects.size(); i++)
		{
			auto renderObject = m_RenderObjects[i];

			if (renderObject->GetVertexBuffer() == nullptr || renderObject->GetIndexBuffer() == nullptr)
				continue;

			ObjectConstants objConstants;
			objConstants.World = renderObject->WorldMatrix.Transpose();

			DynamicUploadBuffer uploadBuffer(m_Device.get(), QueueID::Direct);
			uploadBuffer.LoadData(objConstants);
			uploadBuffer.CreateCBV();

			commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(renderObject->GetVertexBuffer()->GetView()));
			commandContext.GetCmdList()->IASetIndexBuffer(&(renderObject->GetIndexBuffer()->GetView()));
			commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(0, passUploadBuffer.GetAllocation().GPUAddress);
			commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(1, uploadBuffer.GetCBVDescriptorGPUHandle());

			if (renderObject->GetMaterial() && renderObject->GetMaterial()->GetMainTexture())
			{
				D3D12_GPU_DESCRIPTOR_HANDLE texDesc;
				texDesc.ptr = reinterpret_cast<UINT64>(renderObject->GetMaterial()->GetMainTexture()->GetGPUPtr());
				commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(2, texDesc);
			}
			else
			{
				commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(2, m_NullTex->GetGpuHandle());
			}

			commandContext.GetCmdList()->DrawIndexedInstanced(renderObject->GetIndexBuffer()->GetLength(), 1, 0, 0, 0);
		}

		m_DebugRenderer->Render(camera->GetViewProjMatrix(), camera->GetPosition());
	}

	void RenderEngine::EndDraw()
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

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
			m_PendingResize = { (long)x, (long)y, (long)w, (long)h };
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