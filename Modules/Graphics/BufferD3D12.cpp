#include "pch.h"
#include "BufferD3D12.h"

namespace EduEngine
{
	BufferD3D12::BufferD3D12(RenderDeviceD3D12* pDevice, const D3D12_RESOURCE_DESC& desc, QueueID queueId) :
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

		auto& cmdContext = pDevice->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		cmdContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12Resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
		cmdContext.FlushResourceBarriers();
	}

	BufferD3D12::BufferD3D12(RenderDeviceD3D12*			pDevice,
							 const D3D12_RESOURCE_DESC& desc,
							 const void*				initData,
							 UINT64						byteSize,
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

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		hr = m_Device->GetD3D12Device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf()));

		THROW_IF_FAILED(hr, L"Failed to create resource in upload heap");

		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.
		auto& cmdContext = pDevice->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		cmdContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12Resource.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		cmdContext.FlushResourceBarriers();

		cmdContext.UpdateSubresource(m_d3d12Resource.Get(), uploadBuffer.Get(), &subResourceData);

		cmdContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12Resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
		cmdContext.FlushResourceBarriers();

		// Note: uploadBuffer has to be kept alive after the above function calls because
		// the command list has not been executed yet that performs the actual copy.
		// The caller can Release the uploadBuffer after it knows the copy has been executed.

		ReleaseResourceWrapper releaseObject;
		releaseObject.AddResource(std::move(uploadBuffer));
		m_Device->SafeReleaseObject(queueId, std::move(releaseObject));
	}

	void BufferD3D12::CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
	{
		DescriptorHeapAllocation allocation = m_Device->AllocateGPUDescriptor(m_QueueId, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		m_Device->GetD3D12Device()->CreateUnorderedAccessView(m_d3d12Resource.Get(), m_d3d12Resource.Get(), uavDesc, allocation.GetCpuHandle());
		m_UavView = std::make_unique<BufferHeapView>(std::move(allocation));
	}

	BufferHeapView* BufferD3D12::GetView(const D3D12_DESCRIPTOR_HEAP_TYPE& type) const
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return m_UavView.get();
		}

		assert(1);
	}
}