#include "pch.h"
#include "RenderDeviceD3D12.h"
#include "CommandQueueD3D12.h"
#include "CommandContext.h"

namespace EduEngine
{
	RenderDeviceD3D12::RenderDeviceD3D12(Microsoft::WRL::ComPtr<ID3D12Device> device) :
		mDevice(device),
		m_CPUDescriptorHeaps
		{
			{ *this, 64, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE},
			{ *this, 32, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,     D3D12_DESCRIPTOR_HEAP_FLAG_NONE},
			{ *this, 16, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,         D3D12_DESCRIPTOR_HEAP_FLAG_NONE},
			{ *this, 16, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,         D3D12_DESCRIPTOR_HEAP_FLAG_NONE}
		},
		m_GPUDescriptorHeaps
		{
			{ *this, 65536, 32768,		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE},
			{ *this, 1024,  1024 - 128, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,     D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE}
		},
		m_CommandQueues
		{
			{ this, D3D12_COMMAND_LIST_TYPE_DIRECT  },
			{ this, D3D12_COMMAND_LIST_TYPE_COMPUTE }
		},
		m_CommandContexts
		{
			{ *this, D3D12_COMMAND_LIST_TYPE_DIRECT  },
			{ *this, D3D12_COMMAND_LIST_TYPE_COMPUTE }
		},
		m_DynamicSuballocationMgr
		{
			{ m_GPUDescriptorHeaps[0], 2048, "CBV_SRV_UAV_DynSuballocationMgr" },
			{ m_GPUDescriptorHeaps[1], 2048, "SAMPLER_DynSuballocationMgr" }
		},
		m_DynUploadHeap{ true, this, 2048 }
	{
	}

	RenderDeviceD3D12::~RenderDeviceD3D12()
	{
		FinishFrame(true);
	}

	DescriptorHeapAllocation RenderDeviceD3D12::AllocateCPUDescriptor(QueueID queueId, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count)
	{
		assert(type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
		return m_CPUDescriptorHeaps[type].Allocate(queueId, count);
	}

	DescriptorHeapAllocation RenderDeviceD3D12::AllocateGPUDescriptor(QueueID queueId, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count)
	{
		assert(type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && type <= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		return m_GPUDescriptorHeaps[type].Allocate(queueId, count);
	}

	DescriptorHeapAllocation RenderDeviceD3D12::AllocateDynamicDescriptor(QueueID queueId, D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count)
	{
		assert(type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && type <= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		return m_DynamicSuballocationMgr[type].Allocate(queueId, count);
	}

	DynamicAllocation RenderDeviceD3D12::AllocateDynamicUploadGPUDescriptor(QueueID queueId, size_t sizeInBytes)
	{
		assert(queueId >= QueueID::Direct && queueId <= QueueID::Both);
		if (queueId == QueueID::Direct)
			return m_CommandQueues[0].AllocateInDynamicHeap(sizeInBytes);
		else if (queueId == QueueID::Compute)
			return m_CommandQueues[1].AllocateInDynamicHeap(sizeInBytes);
		else if (queueId == QueueID::Both)
			return m_DynUploadHeap.Allocate(sizeInBytes);
	}

	CommandContext& RenderDeviceD3D12::GetCommandContext(D3D12_COMMAND_LIST_TYPE type)
	{
		assert(type == D3D12_COMMAND_LIST_TYPE_DIRECT || type == D3D12_COMMAND_LIST_TYPE_COMPUTE);
		if (type == D3D12_COMMAND_LIST_TYPE_DIRECT)
			return m_CommandContexts[0];
		if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
			return m_CommandContexts[1];
	}

	CommandQueueD3D12& RenderDeviceD3D12::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		assert(type == D3D12_COMMAND_LIST_TYPE_DIRECT || type == D3D12_COMMAND_LIST_TYPE_COMPUTE);
		if (type == D3D12_COMMAND_LIST_TYPE_DIRECT)
			return m_CommandQueues[0];
		if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
			return m_CommandQueues[1];
	}

	void RenderDeviceD3D12::SafeReleaseObject(QueueID queueId, ReleaseResourceWrapper&& wrapper)
	{
		assert(queueId >= QueueID::Direct && queueId <= QueueID::Both);

		if (queueId == QueueID::Direct)
			m_CommandQueues[0].SafeReleaseObject(std::move(wrapper));
		else if (queueId == QueueID::Compute)
			m_CommandQueues[1].SafeReleaseObject(std::move(wrapper));
		else if (queueId == QueueID::Both)
			this->SafeReleaseObject(std::move(wrapper));
	}

	void RenderDeviceD3D12::FinishFrame(bool forceRelease /* = false */)
	{
		for (int i = 0; i < 2; i++)
			m_CommandQueues[i].ProcessReleaseQueue(forceRelease);

		std::lock_guard<std::mutex> LockGuard(m_ReleasedObjectsMutex);

		auto numDirectCompletedCmdLists = m_CommandQueues[0].GetCompletedFenceNum();
		auto numComputeCompletedCmdLists = m_CommandQueues[1].GetCompletedFenceNum();
		auto numDirectNextCmdLists = m_CommandQueues[0].GetNextCmdListNum();
		auto numComputeNextCmdLists = m_CommandQueues[1].GetNextCmdListNum();

		FenceValues completedFences = { numDirectCompletedCmdLists, numComputeCompletedCmdLists };

		while (!m_ReleaseObjectsQueue.empty())
		{
			auto& firstObj = m_ReleaseObjectsQueue.front();
			// GPU must have been idled when ForceRelease == true 
			if (firstObj.first < completedFences || forceRelease)
				m_ReleaseObjectsQueue.pop_front();
			else
				break;
		}

		for (size_t i = 0; i < 2; i++)
			m_DynamicSuballocationMgr[i].DiscardAllocations();

		m_DynUploadHeap.FinishFrame({ numDirectNextCmdLists, numComputeNextCmdLists }, { numDirectCompletedCmdLists, numComputeCompletedCmdLists });
	}

	void RenderDeviceD3D12::FlushQueues()
	{
		for (int i = 0; i < 2; i++)
			m_CommandQueues[i].Flush();
	}

	void RenderDeviceD3D12::SafeReleaseObject(ReleaseResourceWrapper&& wrapper)
	{
		uint64_t directNum = m_CommandQueues[0].GetNextCmdListNum();
		uint64_t computeNum = m_CommandQueues[1].GetNextCmdListNum();

		m_ReleaseObjectsQueue.emplace_back(FenceValues{ directNum, computeNum }, std::move(wrapper));
	}
}