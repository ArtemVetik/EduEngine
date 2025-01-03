#include "pch.h"
#include "../Graphics/DynamicUploadBuffer.h"
#include "RenderEngine.h"
#include "GeometryGenerator.h"

namespace EduEngine
{
	RenderEngine* RenderEngine::m_Instance = nullptr;

	RenderEngine* RenderEngine::GetInstance()
	{
		return m_Instance;
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
#if defined(DEBUG) || defined(_DEBUG) 
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
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

		m_SwapChain = std::make_unique<SwapChain>(m_Device.get(),
			mainWindow.GetClientWidth(), mainWindow.GetClientHeight(), mainWindow.GetMainWindow());
		m_DeferredRendering = std::make_unique<DeferredRendering>(m_Device.get(), m_SwapChain.get());

		m_CSMRendering = std::make_unique<CSMRendering>(m_Device.get(), CSMDistance, 4, CSMSizes, CSMSplits);

		Resize(mainWindow.GetClientWidth(), mainWindow.GetClientHeight());

		m_DebugRenderer = std::make_shared<DebugRendererSystem>(m_Device.get());

		m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).Reset();
		m_SkyBoxRendering = std::make_unique<SkyboxRendering>(m_Device.get());
		m_DeferredRendering->InitResources();

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

	Camera* RenderEngine::CreateCamera()
	{
		auto camera = std::make_shared<CameraInternal>(m_Device.get(), m_SwapChain->GetWidth(), m_SwapChain->GetHeight());
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

	Light* RenderEngine::CreateLight()
	{
		auto light = std::make_shared<Light>();
		light->Strength = { 0.6f, 0.6f, 0.6f };
		light->Direction = { 0.0f, -1.0f, 0.0f };

		m_Lights.emplace_back(light);
		return light.get();
	}

	void RenderEngine::RemoveLight(Light* light)
	{
		m_Lights.erase(std::remove_if(m_Lights.begin(), m_Lights.end(),
			[&light](const std::shared_ptr <Light>& ptr) {
				return ptr.get() == light;
			}),
			m_Lights.end());
	}

	void RenderEngine::UpdateUI(ImDrawData* drawData)
	{
		m_OverlayUI->Update(drawData);
	}

	void* RenderEngine::CreateImGuiUI(void* pixels, int texWidth, int texHeight, int bytesPerPixel)
	{
		m_OverlayUI = std::make_unique<ImGuiD3D12Impl>(m_Device.get(), pixels, texWidth, texHeight, bytesPerPixel);
		return m_OverlayUI->GetFontTexturePtr();
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

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_Device->GetD3D12DescriptorHeap() };
		commandContext.GetCmdList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	}

	void RenderEngine::Draw(Camera* camera)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		D3D12_RECT scissorRect[1] =
		{
			m_Viewport.TopLeftX + m_Viewport.Width * camera->GetViewport().x,
			m_Viewport.TopLeftY + m_Viewport.Height * camera->GetViewport().y,
			m_Viewport.Width * camera->GetViewport().z,
			m_Viewport.Height * camera->GetViewport().w
		};

		for (int i = 0; i < m_Lights.size(); i++)
		{
			if (m_Lights[i]->LightType == Light::Type::Directional)
			{
				m_CSMRendering->Render(camera, m_Lights[i].get(), m_RenderObjects);
				break;
			}
		}

		commandContext.SetViewports(&m_Viewport, 1);
		commandContext.SetScissorRects(scissorRect, 1);

		m_DeferredRendering->PrepareRenderGeometry(camera, m_CSMRendering.get(), scissorRect);

		for (int i = 0; i < m_RenderObjects.size(); i++)
			m_DeferredRendering->RenderGeomerty(m_RenderObjects[i].get());

		for (int i = 0; i < GBufferPass::GBufferCount; i++)
			commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_DeferredRendering->GetGBuffer()->GetGBuffer(i),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->GetDepthStencilBuffer(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
		commandContext.FlushResourceBarriers();

		m_DeferredRendering->RenderLights(camera, m_Lights, scissorRect);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_DeferredRendering->GetGBuffer()->GetAccumBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
		commandContext.FlushResourceBarriers();

		m_DeferredRendering->RenderToneMapping();

		for (int i = 0; i < GBufferPass::GBufferCount; i++)
			commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_DeferredRendering->GetGBuffer()->GetGBuffer(i),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_DeferredRendering->GetGBuffer()->GetAccumBuffer(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->GetDepthStencilBuffer(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		commandContext.FlushResourceBarriers();

		m_SkyBoxRendering->Render(camera);

		if (camera->DebugRenderEnabled())
		{
			commandContext.SetRenderTargets(1, &(m_SwapChain->CurrentBackBufferView()), true, &(m_SwapChain->DepthStencilView()));
			m_DebugRenderer->Render(camera->GetViewProjMatrix(), camera->GetPosition());
		}
	}

	void RenderEngine::EndDraw()
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		if (m_OverlayUI)
		{
			commandContext.SetViewports(&m_Viewport, 1);
			commandContext.SetScissorRects(&m_ScissorRect, 1);

			m_OverlayUI->Draw(m_SwapChain->GetWidth(), m_SwapChain->GetHeight());
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
			m_PendingResize = { (long)x, (long)y, (long)w, (long)h };
	}

	void RenderEngine::Resize(UINT w, UINT h)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();

		m_SwapChain->Resize(w, h);
		m_DeferredRendering->GetGBuffer()->Resize(m_Device.get(), w, h);

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