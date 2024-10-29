#pragma once
#include "pch.h"
#include "VariableSizeMemoryAllocator.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API VariableSizeGPUAllocationsManager : public VariableSizeMemoryAllocator
	{
	public:
		VariableSizeGPUAllocationsManager(size_t maxSize);
		
		void Free(size_t offset, size_t size, uint64_t frameNumber);
		void ReleaseCompletedFrames(uint64_t numCompletedFrames);

	private:
		struct GRAPHICS_HEAPS_API FreedAllocationInfo
		{
			size_t Offset;
			size_t Size;
			uint64_t FrameNumber;

			FreedAllocationInfo(size_t offset, size_t size, uint64_t frameNumber)
				: Offset(offset), Size(size), FrameNumber(frameNumber)
			{}
		};

		std::deque<FreedAllocationInfo> m_StaleAllocations;
	};
}