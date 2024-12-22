#pragma once
#include "pch.h"
#include "DescriptorHeapAllocationManager.h"
#include "ReleaseResourceWrapper.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API CPUDescriptorHeap: public IDescriptorAllocator
	{
	public:
		CPUDescriptorHeap(IRenderDeviceD3D12&		  deviceD3D12Impl,
						  uint32_t                    numDescriptorsInHeap,
						  D3D12_DESCRIPTOR_HEAP_TYPE  type,
						  D3D12_DESCRIPTOR_HEAP_FLAGS flags);

		CPUDescriptorHeap(const CPUDescriptorHeap&) = delete;
		CPUDescriptorHeap(CPUDescriptorHeap&&) = delete;
		CPUDescriptorHeap& operator = (const CPUDescriptorHeap&) = delete;
		CPUDescriptorHeap& operator = (CPUDescriptorHeap&&) = delete;

		~CPUDescriptorHeap();

		virtual DescriptorHeapAllocation Allocate(QueueID queueId, uint32_t count) override;
		virtual void SafeFree(DescriptorHeapAllocation&& allocation) override;
		virtual uint32_t GetDescriptorSize() const override { return m_DescriptorSize; }
		virtual void FreeAllocation(DescriptorHeapAllocation&& allocation) override;

	private:
		std::mutex m_HeapPoolMutex;
		std::vector<DescriptorHeapAllocationManager> m_HeapPool;
		std::set<size_t> m_AvailableHeaps;

		IRenderDeviceD3D12& m_DeviceD3D12Impl;
		D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
		const UINT m_DescriptorSize = 0;

		uint32_t m_MaxSize = 0;
		uint32_t m_CurrentSize = 0;
	};
}