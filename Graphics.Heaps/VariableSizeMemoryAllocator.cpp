#include "pch.h"
#include "VariableSizeMemoryAllocator.h"

namespace EduEngine
{
    VariableSizeMemoryAllocator::VariableSizeMemoryAllocator(size_t maxSize)
        :m_FreeSize(maxSize)
    {
        AddNewBlock(0, maxSize);
    }

    size_t VariableSizeMemoryAllocator::Allocate(size_t size)
    {
        if (m_FreeSize < size)
            return InvalidOffset;

        auto sizeBlockIt = m_FreeBlocksBySize.lower_bound(size);

        if (sizeBlockIt == m_FreeBlocksBySize.end())
            return InvalidOffset;

        auto offsetBlockIt = sizeBlockIt->second;
        auto offset = offsetBlockIt->first;
        auto newOffset = offset + size;
        auto newSize = offsetBlockIt->second.Size - size;

        m_FreeBlocksBySize.erase(sizeBlockIt);
        m_FreeBlocksByOffset.erase(offsetBlockIt);

        if (newSize > 0)
            AddNewBlock(newOffset, newSize);

        m_FreeSize -= size;
        return offset;
    }

    void VariableSizeMemoryAllocator::Free(size_t offset, size_t size)
    {
        auto nextBlockIt = m_FreeBlocksByOffset.upper_bound(offset);
        auto prevBlockIt = nextBlockIt;

        if (prevBlockIt != m_FreeBlocksByOffset.begin())
            --prevBlockIt;
        else
            prevBlockIt = m_FreeBlocksByOffset.end();

        size_t newOffset = 0;
        size_t newSize = 0;

        if (prevBlockIt != m_FreeBlocksByOffset.end() && offset == prevBlockIt->first + prevBlockIt->second.Size)
        {
            // PrevBlock.Offset           Offset
            // |                          |
            // |<-----PrevBlock.Size----->|<------Size-------->|
            //
            newSize = prevBlockIt->second.Size + size;
            newOffset = prevBlockIt->first;

            if (nextBlockIt != m_FreeBlocksByOffset.end() && offset + size == nextBlockIt->first)
            {
                // PrevBlock.Offset           Offset               NextBlock.Offset 
                // |                          |                    |
                // |<-----PrevBlock.Size----->|<------Size-------->|<-----NextBlock.Size----->|
                //
                newSize += nextBlockIt->second.Size;
                m_FreeBlocksBySize.erase(prevBlockIt->second.OrderBySizeIt);
                m_FreeBlocksBySize.erase(nextBlockIt->second.OrderBySizeIt);

                // Delete the range of two blocks
                m_FreeBlocksByOffset.erase(prevBlockIt);
                m_FreeBlocksByOffset.erase(nextBlockIt);
            }
            else
            {
                // PrevBlock.Offset           Offset                       NextBlock.Offset 
                // |                          |                            |
                // |<-----PrevBlock.Size----->|<------Size-------->| ~ ~ ~ |<-----NextBlock.Size----->|
                //
                m_FreeBlocksBySize.erase(prevBlockIt->second.OrderBySizeIt);
                m_FreeBlocksByOffset.erase(prevBlockIt);
            }
        }
        else if (nextBlockIt != m_FreeBlocksByOffset.end() && offset + size == nextBlockIt->first)
        {
            // PrevBlock.Offset                   Offset               NextBlock.Offset 
            // |                                  |                    |
            // |<-----PrevBlock.Size----->| ~ ~ ~ |<------Size-------->|<-----NextBlock.Size----->|
            //
            newSize = size + nextBlockIt->second.Size;
            newOffset = offset;
            m_FreeBlocksBySize.erase(nextBlockIt->second.OrderBySizeIt);
            m_FreeBlocksByOffset.erase(nextBlockIt);
        }
        else
        {
            // PrevBlock.Offset                   Offset                       NextBlock.Offset 
            // |                                  |                            |
            // |<-----PrevBlock.Size----->| ~ ~ ~ |<------Size-------->| ~ ~ ~ |<-----NextBlock.Size----->|
            //
            newSize = size;
            newOffset = offset;
        }

        AddNewBlock(newOffset, newSize);

        m_FreeSize += size;
    }

    void VariableSizeMemoryAllocator::AddNewBlock(size_t offset, size_t size)
    {
        auto newOffsetBlock = m_FreeBlocksByOffset.emplace(offset, size);
        assert(newOffsetBlock.second == true);

        auto newSizeBlock = m_FreeBlocksBySize.emplace(size, newOffsetBlock.first);
        newOffsetBlock.first->second.OrderBySizeIt = newSizeBlock;
    }
}