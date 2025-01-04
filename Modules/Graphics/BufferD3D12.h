#pragma once
#include "pch.h"
#include "ResourceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API BufferHeapView
	{
	private:
		DescriptorHeapAllocation m_Allocation;

	public:
		BufferHeapView(DescriptorHeapAllocation&& allocation) :
			m_Allocation(std::move(allocation))
		{
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t offset = 0) const { return m_Allocation.GetCpuHandle(); }

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t offset = 0) const { return m_Allocation.GetGpuHandle(); }
	};

	class GRAPHICS_API BufferD3D12 : public ResourceD3D12
	{
	public:
		BufferD3D12(RenderDeviceD3D12*		   pDevice,
					const D3D12_RESOURCE_DESC& desc,
					QueueID					   queueId);

		BufferD3D12(RenderDeviceD3D12*		   pDevice,
					const D3D12_RESOURCE_DESC& desc,
					const void*				   initData,
					QueueID					   queueId);

		void CreateCBV();
		void CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
		void CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, bool withCounter = false);

		BufferHeapView* GetCBVView() const;
		BufferHeapView* GetSRVView() const;
		BufferHeapView* GetUAVView() const;

		static inline UINT AlignForUavCounter(UINT bufferSize)
		{
			const UINT alignment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
			return (bufferSize + (alignment - 1)) & ~(alignment - 1);
		}

	private:
		std::unique_ptr<BufferHeapView> m_CbvView;
		std::unique_ptr<BufferHeapView> m_SrvView;
		std::unique_ptr<BufferHeapView> m_UavView;
	};

	class GRAPHICS_API VertexBufferD3D12 : public BufferD3D12
	{
	public:
		VertexBufferD3D12(RenderDeviceD3D12* pDevice,
						  const void*		 initData,
						  UINT				 byteStride,
						  UINT				 bufferLength) :
			BufferD3D12(pDevice, CD3DX12_RESOURCE_DESC::Buffer(byteStride * bufferLength), initData, QueueID::Direct)
		{
			m_View.BufferLocation = m_d3d12Resource->GetGPUVirtualAddress();
			m_View.StrideInBytes = byteStride;
			m_View.SizeInBytes = byteStride * bufferLength;
		}

		D3D12_VERTEX_BUFFER_VIEW GetView() const { return m_View; }

	private:
		D3D12_VERTEX_BUFFER_VIEW m_View;
	};

	class GRAPHICS_API IndexBufferD3D12 : public BufferD3D12
	{
	public:
		IndexBufferD3D12(RenderDeviceD3D12* pDevice,
						 const void*		initData,
						 UINT				byteStride,
						 UINT				bufferLength,
						 DXGI_FORMAT		format) :
			BufferD3D12(pDevice, CD3DX12_RESOURCE_DESC::Buffer(byteStride * bufferLength), initData, QueueID::Direct),
			m_Length(bufferLength)
		{
			m_View.BufferLocation = m_d3d12Resource->GetGPUVirtualAddress();
			m_View.Format = format;
			m_View.SizeInBytes = byteStride * bufferLength;
		}

		D3D12_INDEX_BUFFER_VIEW GetView() const { return m_View; }
		UINT GetLength() const { return m_Length; }

	private:
		UINT m_Length;
		D3D12_INDEX_BUFFER_VIEW m_View;
	};
}

