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

		virtual DescriptorHeapAllocation Allocate(uint32_t count) override;
		virtual void SafeFree(DescriptorHeapAllocation&& allocation) override;
		virtual uint32_t GetDescriptorSize() const override { return m_ParentGPUHeap.GetDescriptorSize(); }

		size_t GetSuballocationCount() const { return m_Suballocations.size(); }

	private:
		GPUDescriptorHeap& m_ParentGPUHeap;
		const char*        m_ManagerName;

		// List of chunks allocated from the master GPU descriptor heap. All chunks are disposed at the end
		// of the frame
		std::vector<DescriptorHeapAllocation> m_Suballocations;

		uint32_t m_CurrentSuballocationOffset = 0;
		uint32_t m_DynamicChunkSize = 0;
	};
}