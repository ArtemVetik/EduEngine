#pragma once
#include "pch.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API DynamicUploadBuffer
	{
	private:
		DynamicAllocation m_DynamicAllocation;
		DescriptorHeapAllocation m_CbvDescriptorAllocation;
		DescriptorHeapAllocation m_SrvDescriptorAllocation;

		RenderDeviceD3D12* m_Device;
		QueueID m_QueueId;

	public:
		DynamicUploadBuffer(RenderDeviceD3D12* pDevice, QueueID queueId) :
			m_Device(pDevice),
			m_QueueId(queueId)
		{
		}

		void CreateAllocation(size_t size);

		template <class T>
		void LoadData(const T& initialData);
		template <class T>
		void PutData(size_t index, const T& data);

		void DynamicUploadBuffer::CreateCBV();
		void DynamicUploadBuffer::CreateSRV(size_t elemCount, size_t byteStride);

		DynamicAllocation GetAllocation() const { return m_DynamicAllocation; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetCBVDescriptorGPUHandle() { return m_CbvDescriptorAllocation.GetGpuHandle(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVDescriptorGPUHandle() { return m_SrvDescriptorAllocation.GetGpuHandle(); }
	};

	template<class T>
	inline void DynamicUploadBuffer::LoadData(const T& initialData)
	{
		m_DynamicAllocation = m_Device->AllocateDynamicUploadGPUDescriptor(m_QueueId, sizeof(T));
		memcpy(m_DynamicAllocation.CPUAddress, &initialData, sizeof(T));
	}

	template<class T>
	inline void DynamicUploadBuffer::PutData(size_t index, const T& data)
	{
		memcpy((T*)m_DynamicAllocation.CPUAddress + index, &data, sizeof(T));
	}
}