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
	}

	CommandQueueD3D12::~CommandQueueD3D12()
	{
		ProcessReleaseQueue(true);
	}

	void CommandQueueD3D12::CloseAndExecuteCommandContext(CommandContext* commandContext)
	{
		assert(m_CommandQueue->GetDesc().Type == commandContext->GetType());

		auto* commandList = commandContext->Close();

		std::lock_guard<std::mutex> LockGuard(m_CmdQueueMutex);

		ID3D12CommandList* const ppCmdLists[] = { commandList };
		m_CommandQueue->ExecuteCommandLists(1, ppCmdLists);

		m_NextCmdList.fetch_add(1);

		m_CommandQueue->Signal(m_Fence.Get(), m_NextCmdList);

		commandContext->DiscardAllocator(m_NextCmdList.load());
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
	}
}