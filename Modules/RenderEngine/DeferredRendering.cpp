#include "pch.h"
#include "DeferredRendering.h"
#include "../Graphics/DynamicUploadBuffer.h"
#include "GeometryGenerator.h"

namespace EduEngine
{
	DeferredRendering::DeferredRendering(RenderDeviceD3D12* device, SwapChain* swapChain) :
		m_Device(device),
		m_SwapChain(swapChain)
	{
		m_GBuffer = std::make_unique<GBuffer>(GBufferPass::GBufferCount, GBufferPass::RtvFormats, 1, DeferredLightPass::AccumBuffFormat);
		m_GBufferPass = std::make_unique<GBufferPass>(m_Device);
		m_DeferredLightPass = std::make_unique<DeferredLightPass>(m_Device);
		m_ToneMappingPass = std::make_unique<ToneMappingPass>(m_Device);
	}

	void DeferredRendering::InitResources()
	{
		GeometryGenerator geoGen;
		NativeMeshData quad = geoGen.CreateQuad(-1, 1, 2, 2, 0);

		m_QuadVertexBuff = std::make_unique<VertexBufferD3D12>(m_Device, quad.GetVerticesPU().data(),
			sizeof(NativeVertexPU), (UINT)quad.GetVerticesPU().size());
		m_QuadIndexBuff = std::make_unique<IndexBufferD3D12>(m_Device, quad.GetIndices16().data(),
			sizeof(uint16), (UINT)quad.GetIndices16().size(), DXGI_FORMAT_R16_UINT);

		D3D12_SHADER_RESOURCE_VIEW_DESC nullSrvDesc = {};
		nullSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		nullSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		nullSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

		auto gpuAlloc = m_Device->AllocateGPUDescriptor(QueueID::Direct, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		m_Device->GetD3D12Device()->CreateShaderResourceView(nullptr, &nullSrvDesc, gpuAlloc.GetCpuHandle());
		m_NullTex = std::make_unique<TextureHeapView>(std::move(gpuAlloc), false);
	}

	void DeferredRendering::PrepareRenderGeometry(Camera* camera, D3D12_RECT* scissorRect)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		D3D12_CPU_DESCRIPTOR_HANDLE gBuffViews[GBufferPass::GBufferCount];
		for (int i = 0; i < GBufferPass::GBufferCount; i++)
			gBuffViews[i] = m_GBuffer->GetGBufferRTVView(i);

		commandContext.SetRenderTargets(GBufferPass::GBufferCount, gBuffViews, false, &(m_SwapChain->DepthStencilView()));

		commandContext.GetCmdList()->ClearDepthStencilView(m_SwapChain->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 1, scissorRect);

		for (int i = 0; i < GBufferPass::GBufferCount; i++)
			commandContext.GetCmdList()->ClearRenderTargetView(m_GBuffer->GetGBufferRTVView(i), DirectX::Colors::Black, 1, scissorRect);

		commandContext.GetCmdList()->SetPipelineState(m_GBufferPass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_GBufferPass->GetD3D12RootSignature());

		GBufferPass::PassConstants passConstants = {};
		XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(camera->GetViewProjMatrix()));
		passConstants.EyePosW = camera->GetPosition();

		DynamicUploadBuffer passUploadBuffer(m_Device, QueueID::Direct);
		passUploadBuffer.LoadData(passConstants);

		commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(3, passUploadBuffer.GetAllocation().GPUAddress);
	}

	void DeferredRendering::RenderGeomerty(const RenderObject* renderObject)
	{
		if (renderObject->GetVertexBuffer() == nullptr ||
			renderObject->GetIndexBuffer() == nullptr ||
			renderObject->GetMaterial() == nullptr)
			return;

		GBufferPass::ObjectConstants objConstants;
		objConstants.World = renderObject->WorldMatrix.Transpose();
		objConstants.TexTransform = renderObject->TextureTransform.Transpose();

		DynamicUploadBuffer objUploadBuffer(m_Device, QueueID::Direct);
		objUploadBuffer.LoadData(objConstants);
		objUploadBuffer.CreateCBV();

		GBufferPass::MaterialConstants matConstants = {};
		matConstants.DiffuseAlbedo = renderObject->GetMaterial()->DiffuseAlbedo;
		matConstants.FresnelR0 = renderObject->GetMaterial()->FresnelR0;
		matConstants.Roughness = renderObject->GetMaterial()->Roughness;

		DynamicUploadBuffer matUploadBuffer(m_Device, QueueID::Direct);
		matUploadBuffer.LoadData(matConstants);
		matUploadBuffer.CreateCBV();

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(renderObject->GetVertexBuffer()->GetView()));
		commandContext.GetCmdList()->IASetIndexBuffer(&(renderObject->GetIndexBuffer()->GetView()));
		commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, objUploadBuffer.GetCBVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(1, matUploadBuffer.GetCBVDescriptorGPUHandle());

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

	void DeferredRendering::RenderLights(Camera* camera, const std::vector<std::shared_ptr<Light>>& lights, const CSMRendering* csmRendering, D3D12_RECT* scissorRect)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.SetRenderTargets(1, &(m_GBuffer->GetAccumBuffRTVView(0)), true, nullptr);

		const float color[4] = { 0, 0, 0, 1 };
		commandContext.GetCmdList()->ClearRenderTargetView(m_SwapChain->CurrentBackBufferView(), color, 1, scissorRect);

		commandContext.GetCmdList()->SetPipelineState(m_DeferredLightPass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_DeferredLightPass->GetD3D12RootSignature());

		commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(m_QuadVertexBuff->GetView()));
		commandContext.GetCmdList()->IASetIndexBuffer(&(m_QuadIndexBuff->GetView()));
		commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, m_GBuffer->GetGBufferSRVView(0));
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(1, m_GBuffer->GetGBufferSRVView(1));
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(2, m_GBuffer->GetGBufferSRVView(2));
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(3, m_SwapChain->DepthStencilSRVView());

		DeferredLightPass::PassConstants lightPassConstants = {};
		auto projInv = XMMatrixInverse(nullptr, (XMLoadFloat4x4(&camera->GetProjectionMatrix())));
		auto viewInv = XMMatrixInverse(nullptr, (XMLoadFloat4x4(&camera->GetViewMatrix())));

		XMStoreFloat4x4(&lightPassConstants.ProjInv, projInv);
		XMStoreFloat4x4(&lightPassConstants.ViewInv, viewInv);
		XMStoreFloat4x4(&lightPassConstants.View, XMMatrixTranspose(XMLoadFloat4x4(&camera->GetViewMatrix())));

		lightPassConstants.EyePosW = camera->GetPosition();
		lightPassConstants.ClearColor = camera->GetBackgroundColor();

		for (size_t i = 0; i < lights.size(); i++)
		{
			if (lights[i]->LightType == Light::Type::Directional)
				lightPassConstants.DirectionalLightsCount++;
			if (lights[i]->LightType == Light::Type::Point)
				lightPassConstants.PointLightsCount++;
			if (lights[i]->LightType == Light::Type::Spotlight)
				lightPassConstants.SpotLightsCount++;
		}
		lightPassConstants.CascadeCount = csmRendering->GetCascadeCount();

		for (int i = 0; i < csmRendering->GetCascadeCount(); i++)
		{
			XMStoreFloat4x4(lightPassConstants.CascadeTransform + i, XMMatrixTranspose(csmRendering->GetCascadeTransform(i)));
			lightPassConstants.CascadeDistance[i] = csmRendering->GetCascadeDistance(i);
		}

		DynamicUploadBuffer lightPassUploadBuffer(m_Device, QueueID::Direct);
		lightPassUploadBuffer.LoadData(lightPassConstants);

		if (lights.size() > 0)
		{
			DynamicUploadBuffer lightsUploadBuffer(m_Device, QueueID::Direct);
			lightsUploadBuffer.CreateAllocation(lights.size() * sizeof(Light));

			int dirIdx = 0;
			int pointIdx = 0;
			int spotIdx = 0;

			for (size_t i = 0; i < lights.size(); i++)
			{
				if (lights[i]->LightType == Light::Type::Directional)
					lightsUploadBuffer.PutData(dirIdx++, *lights[i].get());
				if (lights[i]->LightType == Light::Type::Point)
					lightsUploadBuffer.PutData(lightPassConstants.DirectionalLightsCount + pointIdx++, *lights[i].get());
				if (lights[i]->LightType == Light::Type::Spotlight)
					lightsUploadBuffer.PutData(lightPassConstants.DirectionalLightsCount + lightPassConstants.PointLightsCount + spotIdx++, *lights[i].get());
			}

			lightsUploadBuffer.CreateSRV(lights.size(), sizeof(Light));
			commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(5, lightsUploadBuffer.GetSRVDescriptorGPUHandle());
		}

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(6, csmRendering->GetGPUHandle());
		commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(7, lightPassUploadBuffer.GetAllocation().GPUAddress);

		if (camera->GetSkyGPUPtr())
		{
			D3D12_GPU_DESCRIPTOR_HANDLE textDesc;
			textDesc.ptr = reinterpret_cast<UINT64>(camera->GetSkyGPUPtr());
			commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(4, textDesc);
		}

		commandContext.GetCmdList()->SetGraphicsRoot32BitConstants(8, 4, &camera->GetViewport(), 0);

		commandContext.GetCmdList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}

	void DeferredRendering::RenderToneMapping(Camera* camera)
	{
		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.SetRenderTargets(1, &(m_SwapChain->CurrentBackBufferView()), true, &(m_SwapChain->DepthStencilView()));

		commandContext.GetCmdList()->SetPipelineState(m_ToneMappingPass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_ToneMappingPass->GetD3D12RootSignature());

		commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(m_QuadVertexBuff->GetView()));
		commandContext.GetCmdList()->IASetIndexBuffer(&(m_QuadIndexBuff->GetView()));
		commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, m_GBuffer->GetAccumBuffSRVView(0));
		
		commandContext.GetCmdList()->SetGraphicsRoot32BitConstants(1, 4, &camera->GetViewport(), 0);

		commandContext.GetCmdList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}