#pragma once
#include "pch.h"
#include "../Graphics.Heaps/DynamicUploadHeap.h"
#include "../Graphics.Heaps/ReleaseResourceWrapper.h"
#include "CommandContext.h"

namespace EduEngine
{
	class GRAPHICS_API CommandQueueD3D12
	{
	public:
		CommandQueueD3D12(RenderDeviceD3D12* device, D3D12_COMMAND_LIST_TYPE type);
		~CommandQueueD3D12();

		CommandQueueD3D12(const CommandQueueD3D12&) = delete;
		CommandQueueD3D12(CommandQueueD3D12&&) = delete;
		CommandQueueD3D12& operator = (const CommandQueueD3D12&) = delete;
		CommandQueueD3D12& operator = (CommandQueueD3D12&&) = delete;

		void CloseAndExecuteCommandContext(CommandContext* commandContext);

		void SafeReleaseObject(ReleaseResourceWrapper&& staleObject);
		void ProcessReleaseQueue(bool forceRelease = false);

		void Flush();

		DynamicAllocation AllocateInDynamicHeap(size_t sizeInBytes);

		ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_CommandQueue.Get(); };
		uint64_t GetCompletedFenceNum() { return m_Fence->GetCompletedValue(); }

	private:
		std::mutex m_CmdQueueMutex;
		std::mutex m_ReleasedObjectsMutex;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

		std::atomic<uint64_t> m_NextCmdList;

		typedef std::pair<uint64_t, ReleaseResourceWrapper> ReleaseObject;

		std::unique_ptr<DynamicUploadHeap> m_DynUploadHeap; // must be before m_ReleaseObjectsQueue
		std::deque<ReleaseObject> m_ReleaseObjectsQueue;
	};
}