#pragma once
#include "pch.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API DynamicUploadBuffer
	{
	private:
		DynamicAllocation m_DynamicAllocation;
		DescriptorHeapAllocation m_DescriptorAllocation;

		RenderDeviceD3D12* m_Device;
		QueueID m_QueueId;

	public:
		DynamicUploadBuffer(RenderDeviceD3D12* pDevice, QueueID queueId) :
			m_Device(pDevice),
			m_QueueId(queueId)
		{
		}

		template <class T>
		void LoadData(const T& initialData);

		void DynamicUploadBuffer::CreateCBV();

		DynamicAllocation GetAllocation() const { return m_DynamicAllocation; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetCBVDescriptorGPUHandle() { return m_DescriptorAllocation.GetGpuHandle(); }
	};

	template<class T>
	inline void DynamicUploadBuffer::LoadData(const T& initialData)
	{
		m_DynamicAllocation = m_Device->AllocateDynamicUploadGPUDescriptor(m_QueueId, sizeof(T));
		memcpy(m_DynamicAllocation.CPUAddress, &initialData, sizeof(T));
	}
}