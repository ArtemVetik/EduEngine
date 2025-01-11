#pragma once
#include "pch.h"
#include "ResourceD3D12.h"
#include "RenderDeviceD3D12.h"
#include "../Graphics.Heaps/DescriptorHeapAllocation.h"

namespace EduEngine
{
	class GRAPHICS_API TextureD3D12;

	class GRAPHICS_API TextureHeapView
	{
	private:
		DescriptorHeapAllocation m_Allocation;
		bool m_OnCpu;

	public:
		TextureHeapView(DescriptorHeapAllocation&& allocation, bool onCpu) :
			m_Allocation(std::move(allocation)),
			m_OnCpu(onCpu)
		{
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t offset = 0) const { return m_Allocation.GetCpuHandle(); }

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t offset = 0) const { return m_Allocation.GetGpuHandle(); }

		bool OnCpu() const { return m_OnCpu; }
	};

	class GRAPHICS_API TextureD3D12 : public ResourceD3D12
	{
	public:
		TextureD3D12(RenderDeviceD3D12*		    pDevice,
					 const D3D12_RESOURCE_DESC& resourceDesc,
					 const D3D12_CLEAR_VALUE*	clearValue,
					 QueueID					queueId);

		TextureD3D12(RenderDeviceD3D12* pDevice, Microsoft::WRL::ComPtr<ID3D12Resource> resource, QueueID queueId);
		TextureD3D12(RenderDeviceD3D12* pDevice, std::wstring ddsTexPath, QueueID queueId);
		~TextureD3D12();

		void LoadData(void* dataPtr);

		void CreateSRVView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, bool onCpu);
		void CreateRTVView(const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc, bool onCpu);
		void CreateDSVView(const D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc, bool onCpu);

		TextureHeapView* GetView(const D3D12_DESCRIPTOR_HEAP_TYPE& type) const;

	private:
		DescriptorHeapAllocation Allocate(const D3D12_DESCRIPTOR_HEAP_TYPE& type, bool onCpu);

	private:
		std::unique_ptr<TextureHeapView> m_SrvView;
		std::unique_ptr<TextureHeapView> m_RtvView;
		std::unique_ptr<TextureHeapView> m_DsvView;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_DDSuploadHeap;
	};
}