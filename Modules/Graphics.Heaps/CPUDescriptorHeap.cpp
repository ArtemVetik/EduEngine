#include "pch.h"
#include "CPUDescriptorHeap.h"

namespace EduEngine
{
    CPUDescriptorHeap::CPUDescriptorHeap(IRenderDeviceD3D12&         deviceD3D12Impl,
                                         uint32_t                    numDescriptorsInHeap,
                                         D3D12_DESCRIPTOR_HEAP_TYPE  type,
                                         D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
        m_DeviceD3D12Impl{ deviceD3D12Impl },
        m_HeapDesc
        {
            type,
            numDescriptorsInHeap,
            flags,
            1
        },
        m_DescriptorSize{ deviceD3D12Impl.GetD3D12Device()->GetDescriptorHandleIncrementSize(type) }
    {
        m_HeapPool.emplace_back(m_DeviceD3D12Impl, *this, 0, m_HeapDesc);
        m_AvailableHeaps.insert(0);
    }

    CPUDescriptorHeap::~CPUDescriptorHeap()
    {
        assert(m_CurrentSize == 0); // Not all allocations released
        assert(m_AvailableHeaps.size() == m_HeapPool.size()); // Not all descriptor heap pools are released
    }

    DescriptorHeapAllocation CPUDescriptorHeap::Allocate(QueueID queueId, uint32_t count)
    {
        std::lock_guard<std::mutex> LockGuard(m_HeapPoolMutex);

        DescriptorHeapAllocation allocation;
        // Go through all descriptor heap managers that have free descriptors
        for (auto availableHeapIt = m_AvailableHeaps.begin(); availableHeapIt != m_AvailableHeaps.end(); ++availableHeapIt)
        {
            // Try to allocate descriptors using the current descriptor heap manager
            allocation = m_HeapPool[*availableHeapIt].Allocate(queueId, count);
            // Remove the manager from the pool if it has no more available descriptors
            if (m_HeapPool[*availableHeapIt].GetNumAvailableDescriptors() == 0)
                m_AvailableHeaps.erase(*availableHeapIt);

            // Terminate the loop if descriptor was successfully allocated, otherwise
            // go to the next manager
            if (allocation.GetCpuHandle().ptr != 0)
                break;
        }

        // If there were no available descriptor heap managers or no manager was able 
        // to suffice the allocation request, create a new manager
        if (allocation.GetCpuHandle().ptr == 0)
        {
            // Make sure the heap is large enough to accomodate the requested number of descriptors
            m_HeapDesc.NumDescriptors = std::max(m_HeapDesc.NumDescriptors, static_cast<UINT>(count));
            // Create a new descriptor heap manager. Note that this constructor creates a new D3D12 descriptor
            // heap and references the entire heap. Pool index is used as manager ID
            m_HeapPool.emplace_back(m_DeviceD3D12Impl, *this, m_HeapPool.size(), m_HeapDesc);
            auto NewHeapIt = m_AvailableHeaps.insert(m_HeapPool.size() - 1);

            // Use the new manager to allocate descriptor handles
            allocation = m_HeapPool[*NewHeapIt.first].Allocate(queueId, count);
        }

        m_CurrentSize += (allocation.GetCpuHandle().ptr != 0) ? count : 0;

        return allocation;
    }

    void CPUDescriptorHeap::SafeFree(DescriptorHeapAllocation&& allocation)
    {
        QueueID queueId = allocation.GetQueueID();

        StaleAllocation staleAllocation(
            std::move(allocation),
            *this
        );

        ReleaseResourceWrapper releaseObj;
        releaseObj.AddStaleAllocation(std::move(staleAllocation));
        
        m_DeviceD3D12Impl.SafeReleaseObject(queueId, std::move(releaseObj));
    }

    void CPUDescriptorHeap::FreeAllocation(DescriptorHeapAllocation&& allocation)
    {
        std::lock_guard<std::mutex> LockGuard(m_HeapPoolMutex);

        auto ManagerId = allocation.GetAllocationManagerId();
        m_CurrentSize -= static_cast<uint32_t>(allocation.GetNumHandles());
        m_HeapPool[ManagerId].FreeAllocation(std::move(allocation));
        m_AvailableHeaps.insert(ManagerId);
    }
}