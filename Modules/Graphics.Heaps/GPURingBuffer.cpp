#include "pch.h"
#include "GPURingBuffer.h"

namespace EduEngine
{
	GPURingBuffer::GPURingBuffer(size_t maxSize, ID3D12Device* pd3d12Device, bool allowCPUAccess) :
		RingBuffer(maxSize),
		m_CpuVirtualAddress(nullptr),
		m_GpuVirtualAddress(0)
	{
		D3D12_HEAP_PROPERTIES heapProps;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12_RESOURCE_STATES defaultUsage;
		if (allowCPUAccess)
		{
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			defaultUsage = D3D12_RESOURCE_STATE_GENERIC_READ;
		}
		else
		{
			heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			defaultUsage = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}
		resourceDesc.Width = maxSize;

		HRESULT hr = pd3d12Device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			defaultUsage,
			nullptr,
			IID_PPV_ARGS(&m_pBuffer)
		);

		if (FAILED(hr))
			throw;

		m_pBuffer->SetName(L"Upload Ring Buffer");

		m_GpuVirtualAddress = m_pBuffer->GetGPUVirtualAddress();

		if (allowCPUAccess)
		{
			m_pBuffer->Map(0, nullptr, &m_CpuVirtualAddress);
		}
	}

	GPURingBuffer::GPURingBuffer(GPURingBuffer&& rhs) noexcept :
		RingBuffer(std::move(rhs)),
		m_CpuVirtualAddress(rhs.m_CpuVirtualAddress),
		m_GpuVirtualAddress(rhs.m_GpuVirtualAddress),
		m_pBuffer(std::move(rhs.m_pBuffer))
	{
		rhs.m_CpuVirtualAddress = nullptr;
		rhs.m_GpuVirtualAddress = 0;
	}

	GPURingBuffer& GPURingBuffer::operator=(GPURingBuffer&& rhs) noexcept
	{
		Destroy();

		static_cast<RingBuffer&>(*this) = std::move(rhs);
		m_CpuVirtualAddress = rhs.m_CpuVirtualAddress;
		m_GpuVirtualAddress = rhs.m_GpuVirtualAddress;
		m_pBuffer = std::move(rhs.m_pBuffer);
		rhs.m_CpuVirtualAddress = 0;
		rhs.m_GpuVirtualAddress = 0;

		return *this;
	}

	GPURingBuffer::~GPURingBuffer()
	{
		Destroy();
	}

	DynamicAllocation GPURingBuffer::Allocate(size_t sizeInBytes)
	{
		auto offset = RingBuffer::Allocate(sizeInBytes);

		if (offset != RingBuffer::InvalidOffset)
		{
			DynamicAllocation dynAlloc(m_pBuffer.Get(), offset, sizeInBytes);
			dynAlloc.GPUAddress = m_GpuVirtualAddress + offset;
			dynAlloc.CPUAddress = m_CpuVirtualAddress;
			if (dynAlloc.CPUAddress)
				dynAlloc.CPUAddress = reinterpret_cast<char*>(dynAlloc.CPUAddress) + offset;
			return dynAlloc;
		}
		else
		{
			return DynamicAllocation(nullptr, 0, 0);
		}
	}

	void GPURingBuffer::Destroy()
	{
		if (m_CpuVirtualAddress)
		{
			m_pBuffer->Unmap(0, nullptr);
		}
		m_CpuVirtualAddress = 0;
		m_GpuVirtualAddress = 0;
		m_pBuffer.Reset();
	}
}