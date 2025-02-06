#pragma once
#include "pch.h"
#include "QueueID.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API RingBuffer
	{
	public:
		struct GRAPHICS_HEAPS_API FrameTailAttribs
		{
			FrameTailAttribs(FenceValues fv, size_t off, size_t sz) :
				FenceValue(fv),
				Offset(off),
				Size(sz)
			{}
			// Fence value associated with the command list in which 
			// the allocation could have been referenced last time
			FenceValues FenceValue;
			size_t Offset;
			size_t Size;
		};
		static const size_t InvalidOffset = static_cast<size_t>(-1);

		RingBuffer(size_t maxSize) noexcept;
		RingBuffer(RingBuffer&& rhs) noexcept;
		RingBuffer& operator = (RingBuffer&& rhs) noexcept;

		RingBuffer(const RingBuffer&) = delete;
		RingBuffer& operator = (const RingBuffer&) = delete;

		~RingBuffer();

		size_t Allocate(size_t size);

		void FinishCurrentFrame(FenceValues fenceValue);
		void ReleaseCompletedFrames(FenceValues completedFenceValue);

		size_t GetMaxSize()const { return m_MaxSize; }
		bool IsFull()const { return m_UsedSize == m_MaxSize; };
		bool IsEmpty()const { return m_UsedSize == 0; };
		size_t GetUsedSize()const { return m_UsedSize; }

	private:
		std::deque< FrameTailAttribs > m_CompletedFrameTails;
		size_t m_Head = 0;
		size_t m_Tail = 0;
		size_t m_MaxSize = 0;
		size_t m_UsedSize = 0;
		size_t m_CurrFrameSize = 0;
	};
}