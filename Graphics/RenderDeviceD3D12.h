#pragma once
#include "pch.h"
#include "../Graphics.Heaps/IRenderDeviceD3D12.h"
#include "../Graphics.Heaps/CPUDescriptorHeap.h"
#include "../Graphics.Heaps/GPUDescriptorHeap.h"
#include "../Graphics.Heaps/ReleaseResourceWrapper.h"
#include "../Graphics.Heaps/QueueID.h"
#include "CommandQueueD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API RenderDeviceD3D12 : public IRenderDeviceD3D12
	{
	public:
		RenderDeviceD3D12(Microsoft::WRL::ComPtr<ID3D12Device> device);

		RenderDeviceD3D12(const RenderDeviceD3D12&) = delete;
		RenderDeviceD3D12(RenderDeviceD3D12&&) = delete;
		RenderDeviceD3D12& operator = (const RenderDeviceD3D12&) = delete;
		RenderDeviceD3D12& operator = (RenderDeviceD3D12&&) = delete;

		DescriptorHeapAllocation AllocateCPUDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count);
		DescriptorHeapAllocation AllocateGPUDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count);

		CommandContext& GetCommandContext(D3D12_COMMAND_LIST_TYPE type);
		CommandQueueD3D12& GetCommandQueue(D3D12_COMMAND_LIST_TYPE type);

		virtual void SafeReleaseObject(QueueID queueId, ReleaseResourceWrapper&& wrapper) override;
		void FinishFrame();
		
		void FlushQueues();

		ID3D12Device* GetD3D12Device() override { return mDevice.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

		CPUDescriptorHeap m_CPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		GPUDescriptorHeap m_GPUDescriptorHeaps[2];

		CommandQueueD3D12 m_CommandQueues[2]; // must be after descriptor heaps (release in destructor)
		CommandContext m_CommandContexts[2];
	};
}