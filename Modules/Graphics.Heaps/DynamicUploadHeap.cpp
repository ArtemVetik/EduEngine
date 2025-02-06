#include "pch.h"
#include "DynamicUploadHeap.h"

namespace EduEngine
{
	DynamicUploadHeap::DynamicUploadHeap(bool bIsCPUAccessible, IRenderDeviceD3D12* pDevice, size_t initialSize) :
        m_pDeviceD3D12(pDevice),
        m_bIsCPUAccessible(bIsCPUAccessible)
    {
        m_RingBuffers.emplace_back(initialSize, pDevice->GetD3D12Device(), m_bIsCPUAccessible);
    }

	DynamicAllocation DynamicUploadHeap::Allocate(size_t sizeInBytes, size_t alignment)
	{
        const size_t alignmentMask = alignment - 1;
        // Assert that it's a power of two.
        assert((alignmentMask & alignment) == 0);
        // Align the allocation
        const size_t alignedSize = (sizeInBytes + alignmentMask) & ~alignmentMask;
        auto dynAlloc = m_RingBuffers.back().Allocate(alignedSize);
        if (!dynAlloc.pBuffer)
        {
            // Create new buffer
            auto newMaxSize = m_RingBuffers.back().GetMaxSize() * 2;
            // Make sure the buffer is large enough for the requested chunk
            while (newMaxSize < sizeInBytes) newMaxSize *= 2;
            m_RingBuffers.emplace_back(newMaxSize, m_pDeviceD3D12->GetD3D12Device(), m_bIsCPUAccessible);
            dynAlloc = m_RingBuffers.back().Allocate(alignedSize);
        }
        return dynAlloc;
	}

    void DynamicUploadHeap::FinishFrame(FenceValues fenceValue, FenceValues lastCompletedFenceValue)
    {
        size_t numBuffsToDelete = 0;

        for (size_t ind = 0; ind < m_RingBuffers.size(); ++ind)
        {
            auto& ringBuff = m_RingBuffers[ind];
            ringBuff.FinishCurrentFrame(fenceValue);
            ringBuff.ReleaseCompletedFrames(lastCompletedFenceValue);
            if (numBuffsToDelete == ind && ind < m_RingBuffers.size() - 1 && ringBuff.IsEmpty())
            {
                ++numBuffsToDelete;
            }
        }

        if (numBuffsToDelete)
            m_RingBuffers.erase(m_RingBuffers.begin(), m_RingBuffers.begin() + numBuffsToDelete);
    }
}