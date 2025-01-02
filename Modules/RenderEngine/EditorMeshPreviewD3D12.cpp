#include "pch.h"

#include "EditorMeshPreviewD3D12.h"
#include "CameraInternal.h"
#include "../Graphics/DynamicUploadBuffer.h"

namespace EduEngine
{
	EditorMeshPreviewD3D12::EditorMeshPreviewD3D12(RenderDeviceD3D12* device) :
		m_Device(device),
		m_Width(200),
		m_Height(200)
	{
		m_PreviewCamera = std::make_unique<CameraInternal>(device, m_Width, m_Height);

		D3D_SHADER_MACRO macros[] =
		{
			{ "NO_DIFFUSE_MAP", "1" },
			{ NULL, NULL }
		};
		m_OpaquePass = std::make_unique<OpaquePass>(m_Device, macros);

		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = m_Width;
		m_Viewport.Height = m_Height;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScissorRect = { 0, 0, m_Width, m_Height };

		m_CamRadius = 20.0f;
		m_CamTheta = 0.0f;
		m_CamPhi = XM_PIDIV2;

		Rotate({});
	}

	IEditorRenderEngine::PreviewMeshInfo EditorMeshPreviewD3D12::SetPreviewMesh(const char* filePath)
	{
		m_PreviewMesh.reset();
		m_PreviewMeshRT.reset();
		m_PreviewMeshDSV.reset();

		if (filePath == nullptr)
			return {};

		m_PreviewMesh = std::make_shared<SharedMeshD3D12Impl>(m_Device, filePath);
		m_PreviewMesh->Load();

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resourceDesc.Width = (UINT)m_Width;
		resourceDesc.Height = (UINT)m_Height;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE optClear = {};
		optClear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		optClear.Color[0] = 0;
		optClear.Color[1] = 0;
		optClear.Color[2] = 0;
		optClear.Color[3] = 1;

		m_PreviewMeshRT = std::make_shared<TextureD3D12>(m_Device, resourceDesc, &optClear, QueueID::Direct);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_PreviewMeshRT->GetD3D12Resource()->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_PreviewMeshRT->GetD3D12Resource()->GetDesc().MipLevels;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_PreviewMeshRT->CreateRTVView(nullptr, true);
		m_PreviewMeshRT->CreateSRVView(&srvDesc, false);

		D3D12_RESOURCE_DESC dsDesc = {};
		dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsDesc.Alignment = 0;
		dsDesc.Width = m_Width;
		dsDesc.Height = m_Height;
		dsDesc.DepthOrArraySize = 1;
		dsDesc.MipLevels = 1;
		dsDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE dsClear = {};
		dsClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsClear.DepthStencil.Depth = 1.0f;
		dsClear.DepthStencil.Stencil = 0;

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;

		m_PreviewMeshDSV = std::make_shared<TextureD3D12>(m_Device, dsDesc, &dsClear, QueueID::Direct);
		m_PreviewMeshDSV->CreateDSVView(&dsvDesc, true);

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			m_PreviewMeshRT->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_GENERIC_READ)
		);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			m_PreviewMeshDSV->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE)
		);

		IEditorRenderEngine::PreviewMeshInfo info;
		info.TexturePtr = reinterpret_cast<void*>(m_PreviewMeshRT->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetGpuHandle().ptr);
		info.VertexCount = m_PreviewMesh->GetVertexCount();
		info.IndexCount = m_PreviewMesh->GetIndexCount();

		return info;
	}

	void EditorMeshPreviewD3D12::Rotate(XMFLOAT3 delta)
	{
		m_CamRadius -= delta.z;
		m_CamTheta -= delta.x;
		m_CamPhi -= delta.y;

		m_CamPhi = fmod(m_CamPhi, XM_2PI);
		if (m_CamPhi < 0)
			m_CamPhi += XM_2PI;

		XMFLOAT3 look;
		XMFLOAT3 right;
		XMFLOAT3 up;
		XMFLOAT3 pos;

		float x = m_CamRadius * sinf(m_CamPhi) * cosf(m_CamTheta);
		float y = m_CamRadius * cosf(m_CamPhi);
		float z = m_CamRadius * sinf(m_CamPhi) * sinf(m_CamTheta);
		pos = XMFLOAT3(x, y, z);

		XMFLOAT3 target = XMFLOAT3(0.0f, 0.0f, 0.0f);

		XMVECTOR posVec = XMLoadFloat3(&pos);
		XMVECTOR targetVec = XMLoadFloat3(&target);
		XMVECTOR lookVec = XMVector3Normalize(XMVectorSubtract(targetVec, posVec));
		XMStoreFloat3(&look, lookVec);

		XMVECTOR fixedWorldUp = XMVectorSet(0.0f, m_CamPhi > XM_PI ? -1.0f : 1.0f, 0.0f, 0.0f);

		XMVECTOR rightVec = XMVector3Normalize(XMVector3Cross(fixedWorldUp, lookVec));

		if (XMVector3Equal(rightVec, XMVectorZero()))
			rightVec = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		XMStoreFloat3(&right, rightVec);

		XMVECTOR upVec = XMVector3Normalize(XMVector3Cross(lookVec, rightVec));
		XMStoreFloat3(&up, upVec);

		m_PreviewCamera->Update(look, right, up, pos);
	}

	void EditorMeshPreviewD3D12::Render()
	{
		if (!m_PreviewMesh.get())
			return;

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.SetViewports(&m_Viewport, 1);
		commandContext.SetScissorRects(&m_ScissorRect, 1);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			m_PreviewMeshRT->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		commandContext.FlushResourceBarriers();

		commandContext.SetRenderTargets(1, &(m_PreviewMeshRT->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)->GetCpuHandle()), FALSE,
			&m_PreviewMeshDSV->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)->GetCpuHandle());

		commandContext.GetCmdList()->SetPipelineState(m_OpaquePass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_OpaquePass->GetD3D12RootSignature());

		OpaquePass::PassConstants passConstants = {};
		passConstants.DirectionalLightsCount = 1;
		XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(m_PreviewCamera->GetViewProjMatrix()));
		DynamicUploadBuffer passUploadBuffer(m_Device, QueueID::Direct);
		passUploadBuffer.LoadData(passConstants);

		DynamicUploadBuffer lightUploadBuffer(m_Device, QueueID::Direct);
		Light light = {};
		lightUploadBuffer.LoadData(light);
		lightUploadBuffer.CreateSRV(1, sizeof(Light));

		DynamicUploadBuffer matUploadBuffer(m_Device, QueueID::Direct);
		OpaquePass::MaterialConstants material = {};
		matUploadBuffer.LoadData(material);
		matUploadBuffer.CreateCBV();

		auto renderObject = m_PreviewMesh.get();

		OpaquePass::ObjectConstants objConstants;
		objConstants.World = DirectX::SimpleMath::Matrix::Identity;

		DynamicUploadBuffer objUploadBuffer(m_Device, QueueID::Direct);
		objUploadBuffer.LoadData(objConstants);
		objUploadBuffer.CreateCBV();

		const float bgColor[4] = { 0, 0, 0, 1.0f };
		commandContext.GetCmdList()->ClearRenderTargetView(m_PreviewMeshRT->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)->GetCpuHandle(), bgColor, 0, nullptr);
		commandContext.GetCmdList()->ClearDepthStencilView(m_PreviewMeshDSV->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)->GetCpuHandle(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(renderObject->GetVertexBuffer()->GetView()));
		commandContext.GetCmdList()->IASetIndexBuffer(&(renderObject->GetIndexBuffer()->GetView()));
		commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, objUploadBuffer.GetCBVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(1, matUploadBuffer.GetCBVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(3, lightUploadBuffer.GetSRVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(4, passUploadBuffer.GetAllocation().GPUAddress);

		commandContext.GetCmdList()->DrawIndexedInstanced(renderObject->GetIndexBuffer()->GetLength(), 1, 0, 0, 0);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(
			m_PreviewMeshRT->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_GENERIC_READ)
		);
	}
}