#include "pch.h"
#include "DescriptorHeapAllocation.h"

namespace EduEngine
{
	DescriptorHeapAllocation::DescriptorHeapAllocation() : 
        m_pDescriptorHeap{ nullptr },
        m_NumHandles{ 1 },
        m_DescriptorSize{ 0 },
        m_QueueId { }
    {
        m_FirstCpuHandle.ptr = 0;
        m_FirstGpuHandle.ptr = 0;
    }

    DescriptorHeapAllocation::DescriptorHeapAllocation(IDescriptorAllocator&       allocator,
                                                       ID3D12DescriptorHeap*       heap,
                                                       D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
                                                       D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
                                                       uint32_t                    nHandles,
                                                       uint16_t                    allocationManagerId,
                                                       QueueID                     queueId) :
        m_FirstCpuHandle{ cpuHandle },
        m_FirstGpuHandle{ gpuHandle },
        m_pAllocator{ &allocator },
        m_pDescriptorHeap{ heap },
        m_NumHandles{ nHandles },
        m_AllocationManagerId{ allocationManagerId },
        m_QueueId { queueId }
    {
        assert(m_pAllocator != nullptr && m_pDescriptorHeap != nullptr);
        auto descriptorSize = m_pAllocator->GetDescriptorSize();

        assert(descriptorSize < std::numeric_limits<uint16_t>::max());
        m_DescriptorSize = static_cast<uint16_t>(descriptorSize);
    }

    DescriptorHeapAllocation::DescriptorHeapAllocation(DescriptorHeapAllocation&& allocation) noexcept :
        m_FirstCpuHandle{ std::move(allocation.m_FirstCpuHandle) },
        m_FirstGpuHandle{ std::move(allocation.m_FirstGpuHandle) },
        m_pAllocator{ std::move(allocation.m_pAllocator) },
        m_pDescriptorHeap{ std::move(allocation.m_pDescriptorHeap) },
        m_NumHandles{ std::move(allocation.m_NumHandles) },
        m_AllocationManagerId{ std::move(allocation.m_AllocationManagerId) },
        m_QueueId { std::move(allocation.m_QueueId) },
        m_DescriptorSize{ std::move(allocation.m_DescriptorSize) }
    {
        allocation.Reset();
    }

    DescriptorHeapAllocation& DescriptorHeapAllocation::operator=(DescriptorHeapAllocation&& allocation) noexcept
    {
        m_FirstCpuHandle = std::move(allocation.m_FirstCpuHandle);
        m_FirstGpuHandle = std::move(allocation.m_FirstGpuHandle);
        m_NumHandles = std::move(allocation.m_NumHandles);
        m_pAllocator = std::move(allocation.m_pAllocator);
        m_AllocationManagerId = std::move(allocation.m_AllocationManagerId);
        m_pDescriptorHeap = std::move(allocation.m_pDescriptorHeap);
        m_QueueId = std::move(allocation.m_QueueId);
        m_DescriptorSize = std::move(allocation.m_DescriptorSize);

        allocation.Reset();

        return *this;
    }

    DescriptorHeapAllocation::~DescriptorHeapAllocation()
    {
        if (!IsNull() && m_pAllocator)
            m_pAllocator->SafeFree(std::move(*this));
    }

    void DescriptorHeapAllocation::Reset()
    {
        m_FirstCpuHandle.ptr = 0;
        m_FirstGpuHandle.ptr = 0;
        m_pAllocator = nullptr;
        m_pDescriptorHeap = nullptr;
        m_NumHandles = 0;
        m_AllocationManagerId = InvalidAllocationMgrId;
        m_DescriptorSize = 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapAllocation::GetCpuHandle(uint32_t offset) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = m_FirstCpuHandle;
        
        if (offset != 0)
            CPUHandle.ptr += m_DescriptorSize * offset;
        
        return CPUHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapAllocation::GetGpuHandle(uint32_t offset) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = m_FirstGpuHandle;

        if (offset != 0)
            GPUHandle.ptr += m_DescriptorSize * offset;
        
        return GPUHandle;
    }
}