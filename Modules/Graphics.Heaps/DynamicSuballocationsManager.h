#pragma once
#include "pch.h"
#include "GPUDescriptorHeap.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API DynamicSuballocationsManager : public IDescriptorAllocator
	{
	public:
		DynamicSuballocationsManager(GPUDescriptorHeap& parentGPUHeap,
									 uint32_t           dynamicChunkSize,
									 char*				managerName);

		DynamicSuballocationsManager(const DynamicSuballocationsManager&) = delete;
		DynamicSuballocationsManager(DynamicSuballocationsManager&&) = delete;
		DynamicSuballocationsManager& operator = (const DynamicSuballocationsManager&) = delete;
		DynamicSuballocationsManager& operator = (DynamicSuballocationsManager&&) = delete;

		~DynamicSuballocationsManager();

		void DiscardAllocations();

		virtual DescriptorHeapAllocation Allocate(QueueID queueId, uint32_t count) override;
		virtual void SafeFree(DescriptorHeapAllocation&& allocation) override;
		virtual uint32_t GetDescriptorSize() const override { return m_ParentGPUHeap.GetDescriptorSize(); }
		virtual void FreeAllocation(DescriptorHeapAllocation&& allocation) override { }

	private:
		GPUDescriptorHeap& m_ParentGPUHeap;
		const char*        m_ManagerName;

		// List of chunks allocated from the master GPU descriptor heap. All chunks are disposed at the end
		// of the frame
		std::vector<DescriptorHeapAllocation> m_Suballocations[3];
		uint32_t m_CurrentSuballocationOffset[3] = { 0, 0, 0 };

		uint32_t m_DynamicChunkSize = 0;
	};
}