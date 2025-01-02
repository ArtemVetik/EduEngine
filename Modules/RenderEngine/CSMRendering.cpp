#include "pch.h"
#include "CSMRendering.h"
#include "../Graphics/DynamicUploadBuffer.h"

namespace EduEngine
{
	CSMRendering::CSMRendering(RenderDeviceD3D12* device, float shadowDistance, int count, const XMFLOAT2* sizes, const float* cascadeSplits) :
		m_Device(device),
		m_ShadowDistance(shadowDistance),
		m_CascadeCount(count)
	{
		for (int i = 0; i < m_CascadeCount; i++)
			m_CascadeSplits[i] = cascadeSplits[i];

		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear = {};
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		
		for (size_t i = 0; i < count; i++)
		{
			texDesc.Width = sizes[i].x;
			texDesc.Height = sizes[i].y;

			m_ShadowMaps[i] = std::make_unique<TextureD3D12>(m_Device, texDesc, &optClear, QueueID::Direct);
			m_ShadowMaps[i]->CreateDSVView(&dsvDesc, true);

			commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMaps[i]->GetD3D12Resource(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		}

		for (size_t i = 0; i < count; i++)
			m_ShadowMaps[i]->CreateSRVView(&srvDesc, false);

		m_ShadowMapPass = std::make_unique<ShadowMapPass>(m_Device);
	}

	void CSMRendering::Render(Camera* camera, Light* light, const std::vector<std::shared_ptr<RenderObject>>& renderObjects)
	{
		assert(light->LightType == Light::Type::Directional);

		XMMATRIX lightView = CalculateLightView(light);

		float cascadeNear = 0.0f;

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		for (int i = 0; i < m_CascadeCount; i++)
		{
			float cascadeFar = m_CascadeSplits[i] * m_ShadowDistance;

			XMMATRIX lightProj = CalculateCascadeProjection(cascadeNear, cascadeFar, camera, lightView);

			cascadeNear = cascadeFar;

			// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
			DirectX::XMMATRIX T(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 1.0f);

			m_CascadeTransforms[i] = lightView * lightProj * T;

			ShadowMapPass::PassConstants passConstants;
			XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(XMMatrixMultiply(lightView, lightProj)));

			DynamicUploadBuffer passUploadBuffer(m_Device, QueueID::Direct);
			passUploadBuffer.LoadData(passConstants);
			passUploadBuffer.CreateCBV();

			auto shadowTexDesc = m_ShadowMaps[i]->GetD3D12Resource()->GetDesc();
			D3D12_VIEWPORT viewport = { 0.0f, 0.0f, shadowTexDesc.Width, shadowTexDesc.Height, 0.0f, 1.0f };
			D3D12_RECT scissorRect = { 0.0f, 0.0f, shadowTexDesc.Width, shadowTexDesc.Height };

			commandContext.GetCmdList()->RSSetViewports(1, &viewport);
			commandContext.GetCmdList()->RSSetScissorRects(1, &scissorRect);

			commandContext.SetRenderTargets(0, nullptr, false, &m_ShadowMaps[i]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)->GetCpuHandle());

			commandContext.GetCmdList()->ClearDepthStencilView(m_ShadowMaps[i]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)->GetCpuHandle(),
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

			commandContext.GetCmdList()->SetPipelineState(m_ShadowMapPass->GetD3D12PipelineState());
			commandContext.GetCmdList()->SetGraphicsRootSignature(m_ShadowMapPass->GetD3D12RootSignature());
			commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(1, passUploadBuffer.GetCBVDescriptorGPUHandle());

			for (auto& renderObject : renderObjects)
			{
				if (renderObject->GetVertexBuffer() == nullptr ||
					renderObject->GetIndexBuffer() == nullptr)
					continue;

				ShadowMapPass::ObjectConstants objConstants;
				objConstants.World = renderObject->WorldMatrix.Transpose();

				DynamicUploadBuffer objUploadBuffer(m_Device, QueueID::Direct);
				objUploadBuffer.LoadData(objConstants);
				objUploadBuffer.CreateCBV();

				commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, objUploadBuffer.GetCBVDescriptorGPUHandle());

				commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(renderObject->GetVertexBuffer()->GetView()));
				commandContext.GetCmdList()->IASetIndexBuffer(&(renderObject->GetIndexBuffer()->GetView()));
				commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandContext.GetCmdList()->DrawIndexedInstanced(renderObject->GetIndexBuffer()->GetLength(), 1, 0, 0, 0);
			}

			commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMaps[i]->GetD3D12Resource(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
		}
		commandContext.FlushResourceBarriers();

		for (int i = 0; i < m_CascadeCount; i++)
		{
			commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMaps[i]->GetD3D12Resource(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		}
	}

	XMMATRIX CSMRendering::CalculateLightView(Light* light)
	{
		XMVECTOR lightPos = XMLoadFloat3(&light->Position);
		XMVECTOR lightDir = XMLoadFloat3(&light->Direction);
		XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		if (XMVector3NearEqual(XMLoadFloat3(&light->Direction), upVector, XMVectorReplicate(1e-4f)) ||
			XMVector3NearEqual(XMLoadFloat3(&light->Direction), XMVectorNegate(upVector), XMVectorReplicate(1e-4f)))
		{
			upVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		}

		return XMMatrixLookAtLH(lightPos, XMVectorAdd(lightPos, lightDir), upVector);
	}

	XMMATRIX CSMRendering::CalculateCascadeProjection(float nearDist, float farDist, Camera* camera, XMMATRIX lightView)
	{
		float nx1 = nearDist * tan(camera->GetFovX() / 2.0);
		float fx1 = farDist * tan(camera->GetFovX() / 2.0);
		float ny1 = nearDist * tan(camera->GetFovY() / 2.0);
		float fy1 = farDist * tan(camera->GetFovY() / 2.0);

		XMVECTOR fPoses[8] =
		{
			{ +nx1, +ny1, nearDist, 1 },
			{ -nx1, +ny1, nearDist, 1 },
			{ +nx1, -ny1, nearDist, 1 },
			{ -nx1, -ny1, nearDist, 1 },
			{ +fx1, +fy1, farDist,  1 },
			{ -fx1, +fy1, farDist,  1 },
			{ +fx1, -fy1, farDist,  1 },
			{ -fx1, -fy1, farDist,  1 }
		};

		for (size_t i = 0; i < 8; i++)
		{
			fPoses[i] = DirectX::XMVector3TransformCoord(fPoses[i], DirectX::XMMatrixInverse(nullptr, XMLoadFloat4x4(&camera->GetViewMatrix())));
			fPoses[i] = DirectX::XMVector3TransformCoord(fPoses[i], lightView);
		}

		XMFLOAT3 minCorner;
		XMFLOAT3 maxCorner;

		XMStoreFloat3(&minCorner, fPoses[0]);
		XMStoreFloat3(&maxCorner, fPoses[0]);

		for (size_t i = 1; i < 8; i++)
		{
			DirectX::XMFLOAT3 cornerPos;
			DirectX::XMStoreFloat3(&cornerPos, fPoses[i]);

			if (cornerPos.x < minCorner.x) minCorner.x = cornerPos.x;
			if (cornerPos.y < minCorner.y) minCorner.y = cornerPos.y;
			if (cornerPos.z < minCorner.z) minCorner.z = cornerPos.z;

			if (cornerPos.x > maxCorner.x) maxCorner.x = cornerPos.x;
			if (cornerPos.y > maxCorner.y) maxCorner.y = cornerPos.y;
			if (cornerPos.z > maxCorner.z) maxCorner.z = cornerPos.z;
		}

		float l = minCorner.x;
		float b = minCorner.y;
		float n = minCorner.z - m_ShadowDistance;
		float r = maxCorner.x;
		float t = maxCorner.y;
		float f = maxCorner.z + m_ShadowDistance;

		return DirectX::XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	}
}