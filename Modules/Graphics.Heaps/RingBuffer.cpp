#include "pch.h"
#include "RingBuffer.h"

namespace EduEngine
{
	RingBuffer::RingBuffer(size_t maxSize) noexcept :
		m_MaxSize{ maxSize }
	{
	}

	RingBuffer::RingBuffer(RingBuffer&& rhs) noexcept :
		m_CompletedFrameTails{ std::move(rhs.m_CompletedFrameTails) },
		m_Tail{ rhs.m_Tail },
		m_Head{ rhs.m_Head },
		m_MaxSize{ rhs.m_MaxSize },
		m_UsedSize{ rhs.m_UsedSize },
		m_CurrFrameSize{ rhs.m_CurrFrameSize }
	{
		rhs.m_Tail = 0;
		rhs.m_Head = 0;
		rhs.m_MaxSize = 0;
		rhs.m_UsedSize = 0;
		rhs.m_CurrFrameSize = 0;
	}

	RingBuffer& RingBuffer::operator=(RingBuffer&& rhs) noexcept
	{
		m_CompletedFrameTails = std::move(rhs.m_CompletedFrameTails);
		m_Tail = rhs.m_Tail;
		m_Head = rhs.m_Head;
		m_MaxSize = rhs.m_MaxSize;
		m_UsedSize = rhs.m_UsedSize;
		m_CurrFrameSize = rhs.m_CurrFrameSize;

		rhs.m_MaxSize = 0;
		rhs.m_Tail = 0;
		rhs.m_Head = 0;
		rhs.m_UsedSize = 0;
		rhs.m_CurrFrameSize = 0;

		return *this;
	}

	RingBuffer::~RingBuffer()
	{
		assert(m_UsedSize == 0);
	}

	size_t RingBuffer::Allocate(size_t size)
	{
		if (IsFull())
			return InvalidOffset;

		if (m_Tail >= m_Head)
		{
			//                     Head             Tail     MaxSize
			//                     |                |        |
			//  [                  xxxxxxxxxxxxxxxxx         ]
			//                                         
			//
			if (m_Tail + size <= m_MaxSize)
			{
				auto offset = m_Tail;
				m_Tail += size;
				m_UsedSize += size;
				m_CurrFrameSize += size;
				return offset;
			}
			else if (size <= m_Head)
			{
				// Allocate from the beginning of the buffer
				size_t addSize = (m_MaxSize - m_Tail) + size;
				m_UsedSize += addSize;
				m_CurrFrameSize += addSize;
				m_Tail = size;
				return 0;
			}
		}
		else if (m_Tail + size <= m_Head)
		{
			//
			//       Tail          Head             
			//       |             |             
			//  [xxxx              xxxxxxxxxxxxxxxxxxxxxxxxxx]
			//
			auto offset = m_Tail;
			m_Tail += size;
			m_UsedSize += size;
			m_CurrFrameSize += size;
			return offset;
		}

		return InvalidOffset;
	}

	void RingBuffer::FinishCurrentFrame(FenceValues fenceValue)
	{
		if (m_CurrFrameSize != 0)
		{
			m_CompletedFrameTails.emplace_back(fenceValue, m_Tail, m_CurrFrameSize);
			m_CurrFrameSize = 0;
		}
	}

	void RingBuffer::ReleaseCompletedFrames(FenceValues completedFenceValue)
	{
		// We can release all tails whose associated fence value is less than or equal to CompletedFenceValue
		while (!m_CompletedFrameTails.empty() && m_CompletedFrameTails.front().FenceValue <= completedFenceValue)
		{
			const auto& oldestFrameTail = m_CompletedFrameTails.front();
			assert(oldestFrameTail.Size <= m_UsedSize);
			m_UsedSize -= oldestFrameTail.Size;
			m_Head = oldestFrameTail.Offset;
			m_CompletedFrameTails.pop_front();
		}
	}
}