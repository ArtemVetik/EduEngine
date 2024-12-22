#pragma once
#include "pch.h"
#include "DescriptorHeapAllocation.h"

namespace EduEngine
{
	struct GRAPHICS_HEAPS_API StaleAllocation
	{
		DescriptorHeapAllocation Allocation;
		IDescriptorAllocator* Heap;

		StaleAllocation() :
			Allocation{},
			Heap{ nullptr }
		{
		}

		StaleAllocation(DescriptorHeapAllocation&& _Allocation, IDescriptorAllocator& _Heap) noexcept :
			Allocation{ std::move(_Allocation) },
			Heap{ &_Heap }
		{
		}

		StaleAllocation(const StaleAllocation&) = delete;
		StaleAllocation& operator= (const StaleAllocation&) = delete;

		StaleAllocation& operator= (StaleAllocation&& rhs) noexcept
		{
			Allocation = std::move(rhs.Allocation);
			Heap = std::move(rhs.Heap);

			rhs.Heap = nullptr;

			return *this;
		}

		StaleAllocation(StaleAllocation&& rhs) noexcept :
			Allocation{ std::move(rhs.Allocation) },
			Heap{ rhs.Heap }
		{
			rhs.Heap = nullptr;
		}

		~StaleAllocation()
		{
			if (Heap != nullptr)
				Heap->FreeAllocation(std::move(Allocation));
		}
	};

	class GRAPHICS_HEAPS_API ReleaseResourceWrapper
	{
	public:
		ReleaseResourceWrapper() = default;

		ReleaseResourceWrapper(const ReleaseResourceWrapper&) = delete;
		ReleaseResourceWrapper& operator =(ReleaseResourceWrapper&) = delete;
		ReleaseResourceWrapper& operator =(ReleaseResourceWrapper&& rhs) = delete;

		ReleaseResourceWrapper(ReleaseResourceWrapper&& rhs) noexcept :
			m_Resource{ std::move(rhs.m_Resource) },
			m_StaleAllocation{ std::move(rhs.m_StaleAllocation) }
		{
		}

		void AddResource(Microsoft::WRL::ComPtr<ID3D12Resource>&& resource)
		{
			m_Resource = std::move(resource);
		}

		void AddStaleAllocation(StaleAllocation&& allocation)
		{
			m_StaleAllocation = std::move(allocation);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
		StaleAllocation m_StaleAllocation;
	};
}