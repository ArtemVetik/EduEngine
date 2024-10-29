#include "pch.h"
#include "VariableSizeGPUAllocationsManager.h"

namespace EduEngine
{
	VariableSizeGPUAllocationsManager::VariableSizeGPUAllocationsManager(size_t maxSize)
		: VariableSizeMemoryAllocator(maxSize)
	{ }

	void VariableSizeGPUAllocationsManager::Free(size_t offset, size_t size, uint64_t frameNumber)
	{
		m_StaleAllocations.emplace_back(offset, size, frameNumber);
	}

	void VariableSizeGPUAllocationsManager::ReleaseCompletedFrames(uint64_t numCompletedFrames)
	{
		while (!m_StaleAllocations.empty() && m_StaleAllocations.front().FrameNumber < numCompletedFrames)
		{
			auto& oldAllocation = m_StaleAllocations.front();
			VariableSizeMemoryAllocator::Free(oldAllocation.Offset, oldAllocation.Size);
			m_StaleAllocations.pop_front();
		}
	}
}