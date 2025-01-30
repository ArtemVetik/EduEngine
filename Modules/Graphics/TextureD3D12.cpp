#include "pch.h"
#include "TextureD3D12.h"
#include "DDSTextureLoader.h"

namespace EduEngine
{
	TextureD3D12::TextureD3D12(RenderDeviceD3D12*		  pDevice,
							   const D3D12_RESOURCE_DESC& resourceDesc,
							   const D3D12_CLEAR_VALUE*   clearValue,
							   QueueID					  queueId) :
		ResourceD3D12(pDevice, queueId)
	{
		pDevice->GetD3D12Device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			clearValue,
			IID_PPV_ARGS(m_d3d12Resource.GetAddressOf())
		);
	}

	TextureD3D12::TextureD3D12(RenderDeviceD3D12* pDevice, Microsoft::WRL::ComPtr<ID3D12Resource> resource, QueueID queueId) :
		ResourceD3D12(pDevice, resource, queueId)
	{
	}

	TextureD3D12::TextureD3D12(RenderDeviceD3D12* pDevice, std::wstring ddsTexPath, QueueID queueId) :
		ResourceD3D12(pDevice, queueId)
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile12(
			m_Device->GetD3D12Device(),
			m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).GetCmdList(),
			ddsTexPath.c_str(),
			m_d3d12Resource,
			m_DDSuploadHeap
		);

		THROW_IF_FAILED(hr, L"Failed to load dds texture");
	}

	TextureD3D12::~TextureD3D12()
	{
		if (m_DDSuploadHeap)
		{
			ReleaseResourceWrapper releaseResource;
			releaseResource.AddResource(std::move(m_DDSuploadHeap));

			m_Device->SafeReleaseObject(m_QueueId, std::move(releaseResource));
		}
	}

	void TextureD3D12::LoadData(void* dataPtr)
	{
		UINT64 uploadBufferSize = 0;
		m_Device->GetD3D12Device()->GetCopyableFootprints(&m_d3d12Resource->GetDesc(), 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

		auto uploadBuff = m_Device->AllocateDynamicUploadGPUDescriptor(m_QueueId, uploadBufferSize + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
		int alignOffset = uploadBuff.Offset % D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
		
		memcpy(reinterpret_cast<char*>(uploadBuff.CPUAddress) + alignOffset, dataPtr, uploadBufferSize);

		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource = m_d3d12Resource.Get();
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;
		
		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = uploadBuff.pBuffer;
		src.PlacedFootprint.Offset = uploadBuff.Offset + alignOffset;
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		m_Device->GetD3D12Device()->GetCopyableFootprints(&m_d3d12Resource->GetDesc(), 0, 1, uploadBuff.Offset + alignOffset, &src.PlacedFootprint, nullptr, nullptr, nullptr);
		m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).GetCmdList()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	}

	void TextureD3D12::CreateSRVView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, bool onCpu)
	{
		DescriptorHeapAllocation allocation = std::move(Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, onCpu));
		m_Device->GetD3D12Device()->CreateShaderResourceView(m_d3d12Resource.Get(), srvDesc, allocation.GetCpuHandle());
		m_SrvView = std::make_unique<TextureHeapView>(std::move(allocation), onCpu);
	}

	void TextureD3D12::CreateRTVView(const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc, bool onCpu)
	{
		DescriptorHeapAllocation allocation = std::move(Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, onCpu));
		m_Device->GetD3D12Device()->CreateRenderTargetView(m_d3d12Resource.Get(), rtvDesc, allocation.GetCpuHandle());
		m_RtvView = std::make_unique<TextureHeapView>(std::move(allocation), onCpu);
	}

	void TextureD3D12::CreateDSVView(const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc, bool onCpu)
	{
		DescriptorHeapAllocation allocation = std::move(Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, onCpu));
		m_Device->GetD3D12Device()->CreateDepthStencilView(m_d3d12Resource.Get(), dsvDesc, allocation.GetCpuHandle());
		m_DsvView = std::make_unique<TextureHeapView>(std::move(allocation), onCpu);
	}

	TextureHeapView* TextureD3D12::GetView(const D3D12_DESCRIPTOR_HEAP_TYPE& type) const
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return m_SrvView.get();
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return m_RtvView.get();
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return m_DsvView.get();
		}

		assert(1);
	}

	DescriptorHeapAllocation TextureD3D12::Allocate(const D3D12_DESCRIPTOR_HEAP_TYPE& type, bool onCpu)
	{
		return onCpu ?
			m_Device->AllocateCPUDescriptor(m_QueueId, type, 1) :
			m_Device->AllocateGPUDescriptor(m_QueueId, type, 1);
	}
}