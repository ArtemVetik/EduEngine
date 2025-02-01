#include "pch.h"
#include "BufferD3D12.h"

namespace EduEngine
{
	BufferD3D12::BufferD3D12(RenderDeviceD3D12*			pDevice,
							 const D3D12_RESOURCE_DESC& desc,
							 QueueID					queueId) :
		ResourceD3D12(pDevice, queueId)
	{
		// Create the actual default buffer resource.
		HRESULT hr = m_Device->GetD3D12Device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(m_d3d12Resource.GetAddressOf()));

		THROW_IF_FAILED(hr, L"Failed to create resource in default heap");

		m_d3d12Resource->SetName(L"BufferD3D12");

		auto& cmdContext = pDevice->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		cmdContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12Resource.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_GENERIC_READ));
		cmdContext.FlushResourceBarriers();
	}

	BufferD3D12::BufferD3D12(RenderDeviceD3D12*			pDevice,
							 const D3D12_RESOURCE_DESC& desc,
							 const void*				initData,
							 QueueID					queueId) :
		ResourceD3D12(pDevice, queueId)
	{
		// Create the actual default buffer resource.
		HRESULT hr = m_Device->GetD3D12Device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(m_d3d12Resource.GetAddressOf()));

		THROW_IF_FAILED(hr, L"Failed to create resource in default heap");

		m_d3d12Resource->SetName(L"BufferD3D12"); // TODO: set buffer name

		LoadData(initData);
	}

	void BufferD3D12::LoadData(const void* data, UINT* byteSize /* = nullptr */)
	{
		UINT64 uploadBufferSize = 0;

		if (byteSize)
			uploadBufferSize = *byteSize;
		else
			m_Device->GetD3D12Device()->GetCopyableFootprints(&m_d3d12Resource->GetDesc(), 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

		auto uploadBuff = m_Device->AllocateDynamicUploadGPUDescriptor(m_QueueId, uploadBufferSize);

		memcpy(reinterpret_cast<char*>(uploadBuff.CPUAddress), data, uploadBufferSize);

		auto& cmdContext = m_QueueId == QueueID::Compute
			? m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_COMPUTE)
			: m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		auto beforeState = m_QueueId == QueueID::Compute ? D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_GENERIC_READ;

		cmdContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12Resource.Get(),
			beforeState, D3D12_RESOURCE_STATE_COPY_DEST));
		cmdContext.FlushResourceBarriers();

		cmdContext.GetCmdList()->CopyBufferRegion(m_d3d12Resource.Get(), 0, uploadBuff.pBuffer, uploadBuff.Offset, uploadBufferSize);

		cmdContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12Resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, beforeState));
		cmdContext.FlushResourceBarriers();
	}

	void BufferD3D12::CreateCBV()
	{
		DescriptorHeapAllocation allocation = m_Device->AllocateGPUDescriptor(m_QueueId, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
		desc.BufferLocation = m_d3d12Resource->GetGPUVirtualAddress();
		desc.SizeInBytes = m_d3d12Resource->GetDesc().Width;

		m_Device->GetD3D12Device()->CreateConstantBufferView(&desc, allocation.GetCpuHandle());
		m_CbvView = std::make_unique<BufferHeapView>(std::move(allocation));
	}

	void BufferD3D12::CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
	{
		DescriptorHeapAllocation allocation = m_Device->AllocateGPUDescriptor(m_QueueId, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		m_Device->GetD3D12Device()->CreateShaderResourceView(m_d3d12Resource.Get(), srvDesc, allocation.GetCpuHandle());
		m_SrvView = std::make_unique<BufferHeapView>(std::move(allocation));
	}

	void BufferD3D12::CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
	{
		auto counter = uavDesc->Buffer.CounterOffsetInBytes == 0 ? nullptr : m_d3d12Resource.Get();
		DescriptorHeapAllocation allocation = m_Device->AllocateGPUDescriptor(m_QueueId, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		m_Device->GetD3D12Device()->CreateUnorderedAccessView(m_d3d12Resource.Get(), counter, uavDesc, allocation.GetCpuHandle());
		m_UavView = std::make_unique<BufferHeapView>(std::move(allocation));
	}

	BufferHeapView* BufferD3D12::GetCBVView() const
	{
		return m_CbvView.get();
	}

	BufferHeapView* BufferD3D12::GetSRVView() const
	{
		return m_SrvView.get();
	}

	BufferHeapView* BufferD3D12::GetUAVView() const
	{
		return m_UavView.get();
	}

	UploadBufferD3D12::UploadBufferD3D12(RenderDeviceD3D12*			pDevice,
										 const D3D12_RESOURCE_DESC& desc,
										 QueueID					queueId) :
		ResourceD3D12(pDevice, queueId)
	{
		HRESULT hr = m_Device->GetD3D12Device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_d3d12Resource.GetAddressOf()));

		THROW_IF_FAILED(hr, L"Failed to create resource in upload heap");

		m_d3d12Resource->SetName(L"UploadBufferD3D12");
	}
}