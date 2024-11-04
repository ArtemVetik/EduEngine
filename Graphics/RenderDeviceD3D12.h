#pragma once
#include "pch.h"
#include "../Graphics.Heaps/IRenderDeviceD3D12.h"
#include "../Graphics.Heaps/CPUDescriptorHeap.h"
#include "../Graphics.Heaps/GPUDescriptorHeap.h"
#include "../Graphics.Heaps/DynamicSuballocationsManager.h"
#include "../Graphics.Heaps/ReleaseResourceWrapper.h"
#include "../Graphics.Heaps/QueueID.h"
#include "CommandQueueD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API RenderDeviceD3D12 : public IRenderDeviceD3D12
	{
	public:
		RenderDeviceD3D12(Microsoft::WRL::ComPtr<ID3D12Device> device);
		~RenderDeviceD3D12();

		RenderDeviceD3D12(const RenderDeviceD3D12&) = delete;
		RenderDeviceD3D12(RenderDeviceD3D12&&) = delete;
		RenderDeviceD3D12& operator = (const RenderDeviceD3D12&) = delete;
		RenderDeviceD3D12& operator = (RenderDeviceD3D12&&) = delete;

		DescriptorHeapAllocation AllocateCPUDescriptor(QueueID queueId, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count);
		DescriptorHeapAllocation AllocateGPUDescriptor(QueueID queueId, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count);
		DescriptorHeapAllocation AllocateDynamicDescriptor(QueueID queueId, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count);
		DynamicAllocation AllocateDynamicUploadGPUDescriptor(QueueID queueId, size_t sizeInBytes);

		CommandContext& GetCommandContext(D3D12_COMMAND_LIST_TYPE type);
		CommandQueueD3D12& GetCommandQueue(D3D12_COMMAND_LIST_TYPE type);

		virtual void SafeReleaseObject(QueueID queueId, ReleaseResourceWrapper&& wrapper) override;
		void FinishFrame(bool forceRelease = false);
		
		void FlushQueues();

		ID3D12Device* GetD3D12Device() const override { return mDevice.Get(); }
		ID3D12DescriptorHeap* GetD3D12DescriptorHeap() const { return m_GPUDescriptorHeaps[0].GetD3D12Heap(); }

	private:
		void SafeReleaseObject(ReleaseResourceWrapper&& wrapper);

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

		CPUDescriptorHeap m_CPUDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		GPUDescriptorHeap m_GPUDescriptorHeaps[2];
		DynamicSuballocationsManager m_DynamicSuballocationMgr[2];

		typedef std::pair<uint64_t, ReleaseResourceWrapper> ReleaseObject;

		std::mutex m_ReleasedObjectsMutex;
		DynamicUploadHeap m_DynUploadHeap; // must be before m_ReleaseObjectsQueue
		std::deque<ReleaseObject> m_ReleaseObjectsQueue;

		CommandQueueD3D12 m_CommandQueues[2]; // must be after descriptor heaps (release in destructor)
		CommandContext m_CommandContexts[2];
	};
}