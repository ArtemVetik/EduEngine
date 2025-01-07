#include "pch.h"
#include "CommandQueueD3D12.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	CommandQueueD3D12::CommandQueueD3D12(RenderDeviceD3D12* pDevice, D3D12_COMMAND_LIST_TYPE type) :
		m_NextCmdList(0)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		pDevice->GetD3D12Device()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue));
		pDevice->GetD3D12Device()->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&m_Fence));

		m_CommandQueue->SetName(type == D3D12_COMMAND_LIST_TYPE_DIRECT ? L"DirectCommandQueue" : L"ComputeCommandQueue");

		m_DynUploadHeap = std::make_unique<DynamicUploadHeap>(true, pDevice, 2048);
	}

	CommandQueueD3D12::~CommandQueueD3D12()
	{
		ProcessReleaseQueue(true);
	}

	void CommandQueueD3D12::CloseAndExecuteCommandContext(CommandContext* commandContext)
	{
		assert(m_CommandQueue->GetDesc().Type == commandContext->GetType());

		commandContext->FlushResourceBarriers();

		auto* commandList = commandContext->Close();

		std::lock_guard<std::mutex> LockGuard(m_CmdQueueMutex);

		ID3D12CommandList* const ppCmdLists[] = { commandList };
		m_CommandQueue->ExecuteCommandLists(1, ppCmdLists);

		m_NextCmdList.fetch_add(1);

		m_CommandQueue->Signal(m_Fence.Get(), m_NextCmdList);

		commandContext->DiscardAllocator(m_NextCmdList.load());
	}

	void CommandQueueD3D12::Wait(CommandQueueD3D12* other, UINT64 fenceValue)
	{
		m_CommandQueue->Wait(other->m_Fence.Get(), fenceValue);
	}

	void CommandQueueD3D12::SafeReleaseObject(ReleaseResourceWrapper&& staleObject)
	{
		m_ReleaseObjectsQueue.emplace_back(m_NextCmdList.load(), std::move(staleObject));
	}

	void CommandQueueD3D12::ProcessReleaseQueue(bool forceRelease)
	{
		std::lock_guard<std::mutex> LockGuard(m_ReleasedObjectsMutex);

		auto numCompletedCmdLists = GetCompletedFenceNum();

		while (!m_ReleaseObjectsQueue.empty())
		{
			auto& firstObj = m_ReleaseObjectsQueue.front();
			// GPU must have been idled when ForceRelease == true 
			if (firstObj.first < numCompletedCmdLists || forceRelease)
				m_ReleaseObjectsQueue.pop_front();
			else
				break;
		}

		m_DynUploadHeap->FinishFrame(m_NextCmdList.load(), numCompletedCmdLists);
	}

	void CommandQueueD3D12::Flush()
	{
		m_NextCmdList++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		HRESULT hr = m_CommandQueue->Signal(m_Fence.Get(), m_NextCmdList);
		THROW_IF_FAILED(hr, L"Failed to signal command queue");

		// Wait until the GPU has completed commands up to this fence point.
		if (m_Fence->GetCompletedValue() < m_NextCmdList)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, FALSE, false, EVENT_ALL_ACCESS);

			// fire event when GPU hits current fence  
			hr = m_Fence->SetEventOnCompletion(m_NextCmdList, eventHandle);
			THROW_IF_FAILED(hr, L"Failed to set event on completion");

			// wait until the GPU hits current fence event is fired
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	DynamicAllocation CommandQueueD3D12::AllocateInDynamicHeap(size_t sizeInBytes)
	{
		return m_DynUploadHeap->Allocate(sizeInBytes);
	}
}