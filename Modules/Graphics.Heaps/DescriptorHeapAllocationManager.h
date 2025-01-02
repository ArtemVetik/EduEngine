#pragma once
#include "pch.h"
#include "DescriptorHeapAllocation.h"
#include "VariableSizeMemoryAllocator.h"
#include "IRenderDeviceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API DescriptorHeapAllocationManager
	{
    public:
        DescriptorHeapAllocationManager(IRenderDeviceD3D12&               pDeviceD3D12Impl,
                                        IDescriptorAllocator&             pParentAllocator,
                                        size_t                            ThisManagerId,
                                        const D3D12_DESCRIPTOR_HEAP_DESC& HeapDesc);

        DescriptorHeapAllocationManager(IRenderDeviceD3D12&                          pDeviceD3D12Impl,
                                        IDescriptorAllocator&                        pParentAllocator,
                                        size_t                                       ThisManagerId,
                                        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pd3d12DescriptorHeap,
                                        uint32_t                                     FirstDescriptor,
                                        uint32_t                                     NumDescriptors);

        DescriptorHeapAllocationManager(DescriptorHeapAllocationManager&& rhs) noexcept;

        DescriptorHeapAllocationManager& operator = (DescriptorHeapAllocationManager&& rhs) = delete;
        DescriptorHeapAllocationManager(const DescriptorHeapAllocationManager&) = delete;
        DescriptorHeapAllocationManager& operator = (const DescriptorHeapAllocationManager&) = delete;

        DescriptorHeapAllocation Allocate(QueueID queueId, uint32_t count);
        void FreeAllocation(DescriptorHeapAllocation&& allocation);

        size_t GetNumAvailableDescriptors() const { return m_FreeBlockManager.GetFreeSize(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_DESC desc);

    private:
        VariableSizeMemoryAllocator m_FreeBlockManager;
        D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescriptorHeap;

        D3D12_CPU_DESCRIPTOR_HANDLE m_FirstCPUHandle = { 0 };
        D3D12_GPU_DESCRIPTOR_HANDLE m_FirstGPUHandle = { 0 };

        UINT m_DescriptorSize = 0;

        uint32_t m_NumDescriptorsInAllocation = 0;

        std::mutex m_AllocationMutex;
        IRenderDeviceD3D12& m_DeviceD3D12Impl;
        IDescriptorAllocator& m_ParentAllocator;

        size_t m_ThisManagerId = static_cast<size_t>(-1);
	};
}