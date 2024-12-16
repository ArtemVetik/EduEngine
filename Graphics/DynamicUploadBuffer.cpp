#include "pch.h"
#include "DynamicUploadBuffer.h"

namespace EduEngine
{
	void DynamicUploadBuffer::CreateCBV()
	{
		m_CbvDescriptorAllocation = m_Device->AllocateDynamicDescriptor(m_QueueId, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
		desc.BufferLocation = m_DynamicAllocation.GPUAddress;
		desc.SizeInBytes = m_DynamicAllocation.Size;

		m_Device->GetD3D12Device()->CreateConstantBufferView(&desc, m_CbvDescriptorAllocation.GetCpuHandle());
	}

	void DynamicUploadBuffer::CreateSRV(size_t elemCount, size_t byteStride)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = m_DynamicAllocation.Offset / byteStride;
		srvDesc.Buffer.NumElements = elemCount;
		srvDesc.Buffer.StructureByteStride = byteStride;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_SrvDescriptorAllocation = m_Device->AllocateDynamicDescriptor(m_QueueId, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		m_Device->GetD3D12Device()->CreateShaderResourceView(m_DynamicAllocation.pBuffer, &srvDesc, m_SrvDescriptorAllocation.GetCpuHandle());
	}

	void DynamicUploadBuffer::CreateAllocation(size_t size)
	{
		m_DynamicAllocation = m_Device->AllocateDynamicUploadGPUDescriptor(m_QueueId, size);
	}
}