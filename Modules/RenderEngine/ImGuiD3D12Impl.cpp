#include "pch.h"
#include "ImGuiD3D12Impl.h"
#include "../Graphics/DynamicUploadBuffer.h"

namespace EduEngine
{
	ImGuiD3D12Impl::ImGuiD3D12Impl(RenderDeviceD3D12* device,
								   ImGuiPass*		  renderPass,
								   void*			  pixels,
								   int				  width,
								   int				  height,
								   int				  bytesPerPixel) :
		m_Device(device),
		m_RenderPass(renderPass)
	{
		D3D12_RESOURCE_DESC textureDesc;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Alignment = 0;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		m_FontTexture = std::make_shared<TextureD3D12>(m_Device, textureDesc, nullptr, QueueID::Direct);
		m_FontTexture->SetName(L"ImGuiFontTexture");

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		m_FontTexture->CreateSRVView(&srvDesc, false);

		m_FontTexture->LoadData(pixels);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_FontTexture->GetD3D12Resource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).ResourceBarrier(barrier);
	}

	void ImGuiD3D12Impl::Update(ImDrawData* drawData)
	{
		m_DrawData = drawData;
	}

	void ImGuiD3D12Impl::Draw(float displayWidth, float displayHeight)
	{
		if (m_DrawData == nullptr)
			return;

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto& commandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.GetCmdList()->SetPipelineState(m_RenderPass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_RenderPass->GetD3D12RootSignature());

		ImGuiPass::ProjectionMatrixBuffer projectionConstants;
		XMStoreFloat4x4(&projectionConstants.ProjectionMatrix, DirectX::XMMatrixOrthographicOffCenterLH(0, displayWidth, displayHeight, 0, 0, 1));

		DynamicUploadBuffer projectionBuffer(m_Device, QueueID::Direct);
		projectionBuffer.LoadData(projectionConstants);

		for (int n = 0; n < m_DrawData->CmdListsCount; n++)
		{
			ImDrawList* cmdList = m_DrawData->CmdLists[n];

			auto vbAlloc = m_Device->AllocateDynamicUploadGPUDescriptor(QueueID::Direct, sizeof(ImDrawVert) * (UINT)cmdList->VtxBuffer.Size);
			memcpy(vbAlloc.CPUAddress, (void*)cmdList->VtxBuffer.Data, sizeof(ImDrawVert) * (UINT)cmdList->VtxBuffer.Size);
			D3D12_VERTEX_BUFFER_VIEW vbView = {};
			vbView.BufferLocation = vbAlloc.GPUAddress;
			vbView.StrideInBytes = sizeof(ImDrawVert);
			vbView.SizeInBytes = sizeof(ImDrawVert) * (UINT)cmdList->VtxBuffer.Size;

			auto ibAlloc = m_Device->AllocateDynamicUploadGPUDescriptor(QueueID::Direct, sizeof(ImDrawIdx) * (UINT)cmdList->IdxBuffer.Size);
			memcpy(ibAlloc.CPUAddress, (void*)cmdList->IdxBuffer.Data, sizeof(ImDrawIdx) * (UINT)cmdList->IdxBuffer.Size);
			D3D12_INDEX_BUFFER_VIEW ibView = {};
			ibView.BufferLocation = ibAlloc.GPUAddress;
			ibView.Format = DXGI_FORMAT_R16_UINT;
			ibView.SizeInBytes = sizeof(ImDrawIdx) * (UINT)cmdList->IdxBuffer.Size;

			for (int cmdIndex = 0; cmdIndex < cmdList->CmdBuffer.Size; cmdIndex++)
			{
				ImDrawCmd drawCmd = cmdList->CmdBuffer[cmdIndex];
				
				if (drawCmd.TextureId)
				{
					commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(0, projectionBuffer.GetAllocation().GPUAddress);
					
					D3D12_GPU_DESCRIPTOR_HANDLE textDesc;
					textDesc.ptr = reinterpret_cast<UINT64>(drawCmd.TextureId);

					commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(1, textDesc);
				}

				commandContext.GetCmdList()->IASetVertexBuffers(0, 1, &vbView);
				commandContext.GetCmdList()->IASetIndexBuffer(&ibView);
				commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandContext.GetCmdList()->DrawIndexedInstanced(drawCmd.ElemCount, 1, drawCmd.IdxOffset, drawCmd.VtxOffset, 0);
			}
		}

		m_DrawData = nullptr;
	}
}