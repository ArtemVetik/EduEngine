#include "pch.h"
#include "DynamicUploadBuffer.h"

namespace EduEngine
{
	void DynamicUploadBuffer::CreateCBV()
	{
		m_DescriptorAllocation = m_Device->AllocateDynamicDescriptor(m_QueueId, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

		D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
		desc.BufferLocation = m_DynamicAllocation.GPUAddress;
		desc.SizeInBytes = m_DynamicAllocation.Size;

		m_Device->GetD3D12Device()->CreateConstantBufferView(&desc, m_DescriptorAllocation.GetCpuHandle());
	}
}