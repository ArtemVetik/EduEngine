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
		}
	{
	}

	DescriptorHeapAllocation RenderDeviceD3D12::AllocateCPUDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count)
	{
		assert(type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);
		return m_CPUDescriptorHeaps[type].Allocate(count);
	}

	DescriptorHeapAllocation RenderDeviceD3D12::AllocateGPUDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t count)
	{
		assert(type >= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && type <= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		return m_GPUDescriptorHeaps[type].Allocate(count);
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

		if (queueId == QueueID::Direct || queueId == QueueID::Both)
			m_CommandQueues[0].SafeReleaseObject(std::move(wrapper));
		if (queueId == QueueID::Compute || queueId == QueueID::Both)
			m_CommandQueues[1].SafeReleaseObject(std::move(wrapper));
	}

	void RenderDeviceD3D12::FinishFrame()
	{
		for (int i = 0; i < 2; i++)
			m_CommandQueues[i].ProcessReleaseQueue();
	}

	void RenderDeviceD3D12::FlushQueues()
	{
		for (int i = 0; i < 2; i++)
			m_CommandQueues[i].Flush();
	}
}