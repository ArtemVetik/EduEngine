#pragma once
#include "pch.h"

namespace EduEngine
{
	enum GRAPHICS_HEAPS_API QueueID
	{
		Direct = 0,
		Compute = 1,
		Both = 2,
	};

	struct GRAPHICS_HEAPS_API FenceValues
	{
		uint64_t DirectFence;
		uint64_t ComputeFence;

		bool operator<=(const FenceValues& other) const {
			return (DirectFence <= other.DirectFence) && (ComputeFence <= other.ComputeFence);
		}

		bool operator<(const FenceValues& other) const {
			return (DirectFence < other.DirectFence) && (ComputeFence <= other.ComputeFence);
		}
	};
}