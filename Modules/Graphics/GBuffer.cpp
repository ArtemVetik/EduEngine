#include "GBuffer.h"

namespace EduEngine
{
	GBuffer::GBuffer(int count, const DXGI_FORMAT* formats) :
		m_bufferCount(count)
	{
		for (int i = 0; i < count; i++)
			m_Formats[i] = formats[i];
	}

	void GBuffer::Resize(RenderDeviceD3D12* device, UINT width, UINT height)
	{
		for (int i = 0; i < m_bufferCount; i++)
			m_GBuffers[i].reset();

		D3D12_RESOURCE_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Width = (UINT)width;
		resourceDesc.Height = (UINT)height;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE clearVal;
		clearVal.Color[0] = 0;
		clearVal.Color[1] = 0;
		clearVal.Color[2] = 0;
		clearVal.Color[3] = 1;

		D3D12_SHADER_RESOURCE_VIEW_DESC gBuffDescSRV;
		ZeroMemory(&gBuffDescSRV, sizeof(gBuffDescSRV));
		gBuffDescSRV.Texture2D.MipLevels = 1;
		gBuffDescSRV.Texture2D.MostDetailedMip = 0;
		gBuffDescSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		gBuffDescSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		for (int i = 0; i < m_bufferCount; i++)
		{
			resourceDesc.Format = m_Formats[i];
			clearVal.Format = m_Formats[i];
			gBuffDescSRV.Format = m_Formats[i];

			m_GBuffers[i] = std::make_unique<TextureD3D12>(device, resourceDesc, &clearVal, QueueID::Direct);
			m_GBuffers[i]->CreateRTVView(nullptr, true);
			m_GBuffers[i]->CreateSRVView(&gBuffDescSRV, false);
		}
	}

	ID3D12Resource* GBuffer::GetGBuffer(int index) const
	{
		return m_GBuffers[index]->GetD3D12Resource();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GBuffer::GetGBufferRTVView(int index) const
	{
		return m_GBuffers[index]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)->GetCpuHandle();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GBuffer::GetGBufferSRVView(int index) const
	{
		return m_GBuffers[index]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetGpuHandle();
	}
}