#include "pch.h"
#include "DynamicSuballocationsManager.h"

namespace EduEngine
{
	DynamicSuballocationsManager::DynamicSuballocationsManager(GPUDescriptorHeap& parentGPUHeap,
															   uint32_t			  dynamicChunkSize,
															   char*			  managerName) :
        m_ParentGPUHeap{ parentGPUHeap },
        m_ManagerName{ std::move(managerName) },
        m_DynamicChunkSize { dynamicChunkSize }
    {
    }

    DynamicSuballocationsManager::~DynamicSuballocationsManager()
    {
        assert(m_Suballocations.empty());
    }

    void DynamicSuballocationsManager::DiscardAllocations()
    {
        m_Suballocations.clear();
    }

    DescriptorHeapAllocation DynamicSuballocationsManager::Allocate(uint32_t count)
    {
        // Check if there are no chunks or the last chunk does not have enough space
        if (m_Suballocations.empty() ||
            m_CurrentSuballocationOffset + count > m_Suballocations.back().GetNumHandles())
        {
            // Request new chunk from the GPU descriptor heap
            auto suballocationSize = std::max(m_DynamicChunkSize, count);
            auto newDynamicSubAllocation = m_ParentGPUHeap.AllocateDynamic(suballocationSize);
            m_Suballocations.emplace_back(std::move(newDynamicSubAllocation));
            m_CurrentSuballocationOffset = 0;
        }

        // Perform suballocation from the last chunk
        auto& currentSuballocation = m_Suballocations.back();

        auto managerId = currentSuballocation.GetAllocationManagerId();

        DescriptorHeapAllocation allocation(*this,
            currentSuballocation.GetDescriptorHeap(),
            currentSuballocation.GetCpuHandle(m_CurrentSuballocationOffset),
            currentSuballocation.GetGpuHandle(m_CurrentSuballocationOffset),
            count,
            static_cast<uint16_t>(managerId));
        m_CurrentSuballocationOffset += count;

        return allocation;
    }

    void DynamicSuballocationsManager::SafeFree(DescriptorHeapAllocation&& allocation)
    {
        // Do nothing. Dynamic allocations are not disposed individually, but as whole chunks
        // at the end of the frame by ReleaseAllocations()
        allocation.Reset();
    }
}