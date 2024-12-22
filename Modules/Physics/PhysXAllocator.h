#pragma once
#include <PxPhysicsAPI.h>

namespace EduEngine
{
	class PhysXAllocator : public physx::PxAllocatorCallback
	{
	public:
		virtual void* allocate(size_t size, const char*, const char*, int) override
		{
			void* ptr = _aligned_malloc(size, 16);
			PX_ASSERT((size_t(ptr) & 15) == 0);
			return ptr;
		}

		virtual void deallocate(void* ptr) override
		{
			_aligned_free(ptr);
		}
	};
}