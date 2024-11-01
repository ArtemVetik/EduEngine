#include "pch.h"
#include "BufferD3D12.h"

namespace EduEngine
{
    BufferD3D12::BufferD3D12(RenderDeviceD3D12*         pDevice,
                             const void*                initData,
                             UINT64                     byteSize) :
		ResourceD3D12(pDevice)
    {
		// Create the actual default buffer resource.
		ThrowIfFailed(m_Device->GetD3D12Device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(m_d3d12Resource.GetAddressOf())));

		m_d3d12Resource->SetName(L"BufferD3D12"); // TODO: set buffer name

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		ThrowIfFailed(m_Device->GetD3D12Device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

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
		m_Device->SafeReleaseObject(QueueID::Direct, std::move(releaseObject)); // TODO: queue id
    }
}