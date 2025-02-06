#pragma once
#include "pch.h"
#include "GPURingBuffer.h"
#include "IRenderDeviceD3D12.h"

namespace EduEngine
{
#define DEFAULT_ALIGN 256

	class GRAPHICS_HEAPS_API DynamicUploadHeap
	{
    public:
        DynamicUploadHeap(bool bIsCPUAccessible, class IRenderDeviceD3D12* pDevice, size_t initialSize);

        DynamicUploadHeap(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap(DynamicUploadHeap&&) = delete;
        DynamicUploadHeap& operator=(const DynamicUploadHeap&) = delete;
        DynamicUploadHeap& operator=(DynamicUploadHeap&&) = delete;

        DynamicAllocation Allocate(size_t sizeInBytes, size_t alignment = DEFAULT_ALIGN);

        void FinishFrame(FenceValues fenceValue, FenceValues lastCompletedFenceValue);

    private:
        const bool m_bIsCPUAccessible;
        // When a chunk of dynamic memory is requested, the heap first tries to allocate the memory in the largest GPU buffer. 
        // If allocation fails, it a new ring buffer is created that provides enough space and requests memory from that buffer.
        // Only the largest buffer is used for allocation and all other buffers are released when GPU is done with corresponding frames
        std::vector<GPURingBuffer> m_RingBuffers;
        IRenderDeviceD3D12* m_pDeviceD3D12 = nullptr;
	};
}