#pragma once
#include "pch.h"
#include "DescriptorHeapAllocationManager.h"
#include "ReleaseResourceWrapper.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API GPUDescriptorHeap : public IDescriptorAllocator
	{
	public:
		GPUDescriptorHeap(IRenderDeviceD3D12&		  device,
						  uint32_t                    numDescriptorsInHeap,
						  uint32_t                    numDynamicDescriptors,
						  D3D12_DESCRIPTOR_HEAP_TYPE  type,
						  D3D12_DESCRIPTOR_HEAP_FLAGS flags);

		GPUDescriptorHeap(const GPUDescriptorHeap&) = delete;
		GPUDescriptorHeap(GPUDescriptorHeap&&) = delete;
		GPUDescriptorHeap& operator = (const GPUDescriptorHeap&) = delete;
		GPUDescriptorHeap& operator = (GPUDescriptorHeap&&) = delete;

		DescriptorHeapAllocation AllocateDynamic(QueueID queueId, uint32_t count);

		virtual DescriptorHeapAllocation Allocate(QueueID queueId, uint32_t count) override;
		virtual void SafeFree(DescriptorHeapAllocation&& allocation) override;
		virtual uint32_t GetDescriptorSize() const override { return m_DescriptorSize; }
		virtual void FreeAllocation(DescriptorHeapAllocation&& allocation) override;

		ID3D12DescriptorHeap* GetD3D12Heap() const { return m_pd3d12DescriptorHeap.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_DESC desc);

	private:
		std::mutex m_AllocMutex;
		std::mutex m_DynAllocMutex;

		IRenderDeviceD3D12& m_DeviceD3D12Impl;

		const D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pd3d12DescriptorHeap; // Must be defined after m_HeapDesc

		const UINT m_DescriptorSize;

		DescriptorHeapAllocationManager m_HeapAllocationManager;
		DescriptorHeapAllocationManager m_DynamicAllocationsManager;
	};
}