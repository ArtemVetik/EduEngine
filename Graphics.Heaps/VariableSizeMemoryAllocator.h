#pragma once
#include "pch.h"

namespace EduEngine
{
	class GRAPHICS_HEAPS_API VariableSizeMemoryAllocator
	{
	public:
		VariableSizeMemoryAllocator(size_t maxSize);

		size_t Allocate(size_t size);
		void Free(size_t offset, size_t size);

		size_t GetFreeSize() const { return m_FreeSize; }

		static constexpr const size_t InvalidOffset = static_cast<size_t>(-1);

	private:
		struct FreeBlockInfo;
		typedef std::map<size_t, FreeBlockInfo> TFreeBlocksByOffsetMap;
		typedef std::multimap<size_t, TFreeBlocksByOffsetMap::iterator> TFreeBlocksBySizeMap;

		struct FreeBlockInfo
		{
			size_t Size;
			TFreeBlocksBySizeMap::iterator OrderBySizeIt;

			FreeBlockInfo(size_t size) : Size(size) {}
		};

		void AddNewBlock(size_t offset, size_t size);

		TFreeBlocksByOffsetMap m_FreeBlocksByOffset;
		TFreeBlocksBySizeMap m_FreeBlocksBySize;
		size_t m_FreeSize;
	};
}