#include "pch.h"
#include "DescriptorHeapAllocationManager.h"
#include "../Graphics/framework.h"

namespace EduEngine
{
	DescriptorHeapAllocationManager::DescriptorHeapAllocationManager(IRenderDeviceD3D12&			   deviceD3D12Impl,
																	 IDescriptorAllocator&			   parentAllocator,
																	 size_t							   thisManagerId,
																	 const D3D12_DESCRIPTOR_HEAP_DESC& heapDesc) :
		DescriptorHeapAllocationManager
		(
			deviceD3D12Impl,
			parentAllocator,
			thisManagerId,
			CreateHeap(deviceD3D12Impl.GetD3D12Device(), heapDesc),
			0,                      // First descriptor
			heapDesc.NumDescriptors // Num descriptors
		)
	{ }

	DescriptorHeapAllocationManager::DescriptorHeapAllocationManager(IRenderDeviceD3D12&						  deviceD3D12Impl,
																	 IDescriptorAllocator&						  parentAllocator,
																	 size_t										  thisManagerId,
																	 Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> d3d12DescriptorHeap,
																	 uint32_t									  firstDescriptor,
																	 uint32_t									  numDescriptors) :
		m_ParentAllocator(parentAllocator),
		m_DeviceD3D12Impl(deviceD3D12Impl),
		m_ThisManagerId(thisManagerId),
		m_HeapDesc(d3d12DescriptorHeap->GetDesc()),
		m_DescriptorSize(deviceD3D12Impl.GetD3D12Device()->GetDescriptorHandleIncrementSize(m_HeapDesc.Type)),
		m_NumDescriptorsInAllocation(numDescriptors),
		m_pDescriptorHeap(d3d12DescriptorHeap),
		m_FreeBlockManager(numDescriptors)
	{
		m_FirstCPUHandle = d3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_FirstCPUHandle.ptr += SIZE_T{ m_DescriptorSize } * SIZE_T{ firstDescriptor };

		if (m_HeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			m_FirstGPUHandle = d3d12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			m_FirstGPUHandle.ptr += SIZE_T{ m_DescriptorSize } * SIZE_T{ firstDescriptor };
		}
	}

	DescriptorHeapAllocationManager::DescriptorHeapAllocationManager(DescriptorHeapAllocationManager&& rhs) noexcept :
		m_ParentAllocator{ rhs.m_ParentAllocator },
		m_DeviceD3D12Impl{ rhs.m_DeviceD3D12Impl },
		m_ThisManagerId{ rhs.m_ThisManagerId },
		m_HeapDesc{ rhs.m_HeapDesc },
		m_DescriptorSize{ rhs.m_DescriptorSize },
		m_NumDescriptorsInAllocation{ rhs.m_NumDescriptorsInAllocation },
		// Mutex is not movable
		//m_FreeBlockManagerMutex     (std::move(rhs.m_FreeBlockManagerMutex))
		m_FreeBlockManager{ std::move(rhs.m_FreeBlockManager) },
		m_pDescriptorHeap{ std::move(rhs.m_pDescriptorHeap) },
		m_FirstCPUHandle{ rhs.m_FirstCPUHandle },
		m_FirstGPUHandle{ rhs.m_FirstGPUHandle }
	{
		rhs.m_NumDescriptorsInAllocation = 0; // Must be set to zero so that debug check in dtor passes
		rhs.m_ThisManagerId = static_cast<size_t>(-1);
		rhs.m_FirstCPUHandle.ptr = 0;
		rhs.m_FirstGPUHandle.ptr = 0;
	}

	DescriptorHeapAllocation DescriptorHeapAllocationManager::Allocate(QueueID queueId, uint32_t count)
	{
		std::lock_guard<std::mutex> LockGuard(m_AllocationMutex);

		auto descriptorHandleOffset = m_FreeBlockManager.Allocate(count);

		if (descriptorHandleOffset == VariableSizeMemoryAllocator::InvalidOffset)
			return DescriptorHeapAllocation();

		auto CPUHandle = m_FirstCPUHandle;
		CPUHandle.ptr += descriptorHandleOffset * m_DescriptorSize;

		auto GPUHandle = m_FirstGPUHandle;
		if (m_HeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			GPUHandle.ptr += descriptorHandleOffset * m_DescriptorSize;

		return DescriptorHeapAllocation(m_ParentAllocator, m_pDescriptorHeap.Get(), CPUHandle, GPUHandle, count, static_cast<uint16_t>(m_ThisManagerId), queueId);
	}

	void DescriptorHeapAllocationManager::FreeAllocation(DescriptorHeapAllocation&& allocation)
	{
		std::lock_guard<std::mutex> LockGuard(m_AllocationMutex);
		auto descriptorOffset = (allocation.GetCpuHandle().ptr - m_FirstCPUHandle.ptr) / m_DescriptorSize;

		m_FreeBlockManager.Free(descriptorOffset, allocation.GetNumHandles());
		allocation.Reset();
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapAllocationManager::CreateHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_DESC desc)
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pd3d12DescriptorHeap;
		HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(pd3d12DescriptorHeap.GetAddressOf()));

		if (FAILED(hr))
			throw;

		return pd3d12DescriptorHeap;
	}
}