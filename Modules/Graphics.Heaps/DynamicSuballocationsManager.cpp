#include "pch.h"
#include "DynamicSuballocationsManager.h"

namespace EduEngine
{
	DynamicSuballocationsManager::DynamicSuballocationsManager(GPUDescriptorHeap& parentGPUHeap,
															   uint32_t			  dynamicChunkSize,
															   char*			  managerName) :
        m_ParentGPUHeap{ parentGPUHeap },
        m_DynamicChunkSize { dynamicChunkSize },
        m_ManagerName{ std::move(managerName) }
    {
    }

    DynamicSuballocationsManager::~DynamicSuballocationsManager()
    {
        for (size_t i = 0; i < 3; i++)
            assert(m_Suballocations[i].empty());
    }

    void DynamicSuballocationsManager::DiscardAllocations()
    {
        for (size_t i = 0; i < 3; i++)
            m_Suballocations[i].clear();
    }

    DescriptorHeapAllocation DynamicSuballocationsManager::Allocate(QueueID queueId, uint32_t count)
    {
        assert(queueId >= QueueID::Direct && queueId <= QueueID::Both);

        // Check if there are no chunks or the last chunk does not have enough space
        if (m_Suballocations[queueId].empty() || m_CurrentSuballocationOffset[queueId] + count > m_Suballocations[queueId].back().GetNumHandles())
        {
            // Request new chunk from the GPU descriptor heap
            auto suballocationSize = std::max(m_DynamicChunkSize, count);
            auto newDynamicSubAllocation = m_ParentGPUHeap.AllocateDynamic(queueId, suballocationSize);
            m_Suballocations[queueId].emplace_back(std::move(newDynamicSubAllocation));
            m_CurrentSuballocationOffset[queueId] = 0;
        }

        // Perform suballocation from the last chunk
        auto& currentSuballocation = m_Suballocations[queueId].back();

        auto managerId = currentSuballocation.GetAllocationManagerId();

        DescriptorHeapAllocation allocation(*this,
            currentSuballocation.GetDescriptorHeap(),
            currentSuballocation.GetCpuHandle(m_CurrentSuballocationOffset[queueId]),
            currentSuballocation.GetGpuHandle(m_CurrentSuballocationOffset[queueId]),
            count,
            static_cast<uint16_t>(managerId),
            queueId);
        m_CurrentSuballocationOffset[queueId] += count;

        return allocation;
    }

    void DynamicSuballocationsManager::SafeFree(DescriptorHeapAllocation&& allocation)
    {
        // Do nothing. Dynamic allocations are not disposed individually, but as whole chunks
        // at the end of the frame by ReleaseAllocations()
        allocation.Reset();
    }
}