#include "pch.h"
#include "../EduMath/SimpleMath.h"
#include "../Graphics/DynamicUploadBuffer.h"
#include "SkyboxRendering.h"
#include "GeometryGenerator.h"

namespace EduEngine
{
	SkyboxRendering::SkyboxRendering(RenderDeviceD3D12* device) :
		m_Device(device)
	{
		m_SkyboxPass = std::make_unique<SkyboxPass>(m_Device);

		GeometryGenerator geoGen;
		NativeMeshData sphereMeshData = geoGen.CreateSphere(1, 20, 20);

		m_SphereVB = std::make_unique<VertexBufferD3D12>(m_Device, sphereMeshData.Vertices.data(),
			sizeof(NativeVertex), sphereMeshData.Vertices.size());
		m_SphereIB = std::make_unique<IndexBufferD3D12>(m_Device, sphereMeshData.GetIndices16().data(),
			sizeof(uint16), sphereMeshData.GetIndices16().size(), DXGI_FORMAT_R16_UINT);
	}

	void SkyboxRendering::Render(Camera* camera)
	{
		auto skyTexture = camera->GetSkyGPUPtr();

		if (skyTexture == nullptr)
			return;

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.GetCmdList()->SetPipelineState(m_SkyboxPass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_SkyboxPass->GetD3D12RootSignature());

		SkyboxPass::CB cb;
		DirectX::XMStoreFloat4x4(&cb.World, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f));
		DirectX::XMStoreFloat4x4(&cb.ViewProj, XMMatrixTranspose(camera->GetViewProjMatrix()));
		cb.EyePosW = camera->GetPosition();

		DynamicUploadBuffer cbUploadBuffer(m_Device, QueueID::Direct);
		cbUploadBuffer.LoadData(cb);

		D3D12_GPU_DESCRIPTOR_HANDLE textDesc;
		textDesc.ptr = reinterpret_cast<UINT64>(skyTexture);

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, textDesc);
		commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(1, cbUploadBuffer.GetAllocation().GPUAddress);

		commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &(m_SphereVB->GetView()));
		commandContext.GetCmdList()->IASetIndexBuffer(&(m_SphereIB->GetView()));
		commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandContext.GetCmdList()->DrawIndexedInstanced(m_SphereIB->GetLength(), 1, 0, 0, 0);
	}
}