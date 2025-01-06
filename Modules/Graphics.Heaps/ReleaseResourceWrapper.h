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
			m_RootSignature{ std::move(rhs.m_RootSignature) },
			m_Pageable{ std::move(rhs.m_Pageable) },
			m_StaleAllocation{ std::move(rhs.m_StaleAllocation) }
		{
		}

		void AddResource(Microsoft::WRL::ComPtr<ID3D12Resource>&& resource)
		{
			m_Resource = std::move(resource);
		}

		void AddRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>&& signature)
		{
			m_RootSignature = std::move(signature);
		}

		void AddPageable(Microsoft::WRL::ComPtr<ID3D12Pageable>&& pageable)
		{
			m_Pageable = std::move(pageable);
		}

		void AddStaleAllocation(StaleAllocation&& allocation)
		{
			m_StaleAllocation = std::move(allocation);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12Pageable> m_Pageable;
		StaleAllocation m_StaleAllocation;
	};
}