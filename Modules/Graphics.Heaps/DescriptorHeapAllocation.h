#pragma once
#include "pch.h"
#include "QueueID.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API DescriptorHeapAllocation;
	class GRAPHICS_HEAPS_API IDescriptorAllocator;

	class GRAPHICS_HEAPS_API IDescriptorAllocator
	{
	public:
		virtual DescriptorHeapAllocation Allocate(QueueID queueId, uint32_t count) = 0;
		virtual void SafeFree(DescriptorHeapAllocation&& allocation) = 0;
		virtual uint32_t GetDescriptorSize() const = 0;
		virtual void FreeAllocation(DescriptorHeapAllocation&& allocation) = 0;

		virtual ~IDescriptorAllocator() {}
	};

	class GRAPHICS_HEAPS_API DescriptorHeapAllocation
	{
	public:
		DescriptorHeapAllocation();

		DescriptorHeapAllocation(IDescriptorAllocator&		 allocator,
								 ID3D12DescriptorHeap*		 heap,
								 D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
								 D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
								 uint32_t                    nHandles,
								 uint16_t                    allocationManagerId,
								 QueueID queueId);

		DescriptorHeapAllocation(DescriptorHeapAllocation&& allocation) noexcept;

		DescriptorHeapAllocation& operator = (DescriptorHeapAllocation&& allocation) noexcept;

		~DescriptorHeapAllocation();

		void Reset();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t offset = 0) const;

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t offset = 0) const;

		ID3D12DescriptorHeap* GetDescriptorHeap() { return m_pDescriptorHeap; }

		size_t GetNumHandles()const { return m_NumHandles; }

		bool IsNull() const { return m_FirstCpuHandle.ptr == 0; }
		bool IsShaderVisible() const { return m_FirstGpuHandle.ptr != 0; }
		size_t GetAllocationManagerId() { return m_AllocationManagerId; }
		uint16_t GetDescriptorSize() const { return m_DescriptorSize; }
		QueueID GetQueueID() const { return m_QueueId; }

		static constexpr uint16_t InvalidAllocationMgrId = 0xFFFF;

	private:
		DescriptorHeapAllocation(const DescriptorHeapAllocation&) = delete;
		DescriptorHeapAllocation& operator= (const DescriptorHeapAllocation&) = delete;

		D3D12_CPU_DESCRIPTOR_HANDLE m_FirstCpuHandle = { 0 };
		D3D12_GPU_DESCRIPTOR_HANDLE m_FirstGpuHandle = { 0 };
		IDescriptorAllocator* m_pAllocator = nullptr;
		ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr;
		uint32_t m_NumHandles = 0;
		uint16_t m_AllocationManagerId = static_cast<uint16_t>(-1);
		uint16_t m_DescriptorSize = 0;
		QueueID m_QueueId;
	};
}