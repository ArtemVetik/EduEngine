#pragma once
#include "pch.h"
#include "ResourceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API BufferD3D12 : public ResourceD3D12
	{
	public:
		BufferD3D12(RenderDeviceD3D12*		   pDevice,
					const void*				   initData,
					UINT64					   byteSize,
					QueueID					   queueId);
	};

	class GRAPHICS_API VertexBufferD3D12 : public BufferD3D12
	{
	public:
		VertexBufferD3D12(RenderDeviceD3D12* pDevice,
						  const void*		 initData,
						  UINT				 byteStride,
						  UINT				 bufferLength) :
			BufferD3D12(pDevice, initData, byteStride * bufferLength, QueueID::Direct)
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
			BufferD3D12(pDevice, initData, byteStride * bufferLength, QueueID::Direct),
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

