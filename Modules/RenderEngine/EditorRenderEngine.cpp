#include "pch.h"
#include "EditorRenderEngine.h"

namespace EduEngine
{
	EditorRenderEngine* EditorRenderEngine::m_Instance = nullptr;

	EditorRenderEngine::EditorRenderEngine() :
		m_Viewport{},
		m_ScissorRect{}
	{
		assert(m_Instance == nullptr);
		m_Instance = this;
	}

	EditorRenderEngine::~EditorRenderEngine()
	{
		if (m_Device != nullptr)
			m_Device->FlushQueues();
	}

	bool EditorRenderEngine::StartUp(const EditorWindow& editorWindow)
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
			editorWindow.GetClientWidth(), editorWindow.GetClientHeight(), editorWindow.GetMainWindow(), true);

		Resize(editorWindow.GetClientWidth(), editorWindow.GetClientHeight());

		m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).Reset();

		m_PreviewMesh = std::make_shared<EditorMeshPreviewD3D12>(m_Device.get());

		return true;
	}

	void EditorRenderEngine::Resize(UINT width, UINT height)
	{
		m_Device->FlushQueues();

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

		m_ScissorRect = { 0, 0, (int)width, (int)height };
	}

	void EditorRenderEngine::Draw()
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.Reset();

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		commandContext.FlushResourceBarriers();

		FLOAT clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		commandContext.GetCmdList()->ClearRenderTargetView(m_SwapChain->CurrentBackBufferView(), clearColor, 0, nullptr);

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_Device->GetD3D12DescriptorHeap() };
		commandContext.GetCmdList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		m_PreviewMesh->Render();

		commandContext.SetRenderTargets(1, &(m_SwapChain->CurrentBackBufferView()), FALSE, nullptr);

		commandContext.SetViewports(&m_Viewport, 1);
		commandContext.SetScissorRects(&m_ScissorRect, 1);

		if (m_EditorUI)
			m_EditorUI->Draw(m_SwapChain->GetWidth(), m_SwapChain->GetHeight());

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_SwapChain->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		commandContext.FlushResourceBarriers();
		commandQueue.CloseAndExecuteCommandContext(&commandContext);

		commandContext.Reset();

		m_SwapChain->Present();
		m_Device->FinishFrame();
	}

	void EditorRenderEngine::UpdateEditor(ImDrawData* drawData)
	{
		m_EditorUI->Update(drawData);
	}

	void* EditorRenderEngine::CreateEditorImGuiUI(void* pixels, int texWidth, int texHeight, int bytesPerPixel)
	{
		m_EditorUI = std::make_unique<ImGuiD3D12Impl>(m_Device.get(), pixels, texWidth, texHeight, bytesPerPixel);
		return m_EditorUI->GetFontTexturePtr();
	}

	DirectX::SimpleMath::Vector2 EditorRenderEngine::GetScreenSize() const
	{
		return DirectX::SimpleMath::Vector2(m_SwapChain->GetWidth(), m_SwapChain->GetHeight());
	}

	void* EditorRenderEngine::SetPreviewTexture(const wchar_t* filePath)
	{
		if (m_PreviewTex.get())
			m_PreviewTex.reset();

		if (filePath == nullptr)
			return nullptr;

		m_PreviewTex = std::make_shared<TextureD3D12Impl>(m_Device.get(), filePath);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = -1;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = 1;
		srvDesc.Texture2DArray.PlaneSlice = 0;

		m_PreviewTex->Load(&srvDesc);

		return m_PreviewTex->GetGPUPtr();
	}

	IEditorRenderEngine::PreviewMeshInfo EditorRenderEngine::SetPreviewMesh(const char* filePath)
	{
		return m_PreviewMesh->SetPreviewMesh(filePath);
	}

	void EditorRenderEngine::RotatePreviewMesh(DirectX::XMFLOAT3 delta)
	{
		m_PreviewMesh->Rotate(delta);
	}
}