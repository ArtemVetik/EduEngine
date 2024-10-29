#include "pch.h"
#include "CommandListManager.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	CommandListManager::CommandListManager(RenderDeviceD3D12& device, const D3D12_COMMAND_LIST_TYPE& listType) :
		m_Device(device),
		m_CmdListType(listType)
	{
	}

	void CommandListManager::CreateNewCommandList(ID3D12GraphicsCommandList** List, ID3D12CommandAllocator** Allocator)
	{
		RequestCommandAllocator(Allocator);
		
		auto* pd3d12Device = m_Device.GetD3D12Device();
		auto hr = pd3d12Device->CreateCommandList(1, m_CmdListType, *Allocator, nullptr, __uuidof(*List), reinterpret_cast<void**>(List));
		assert(SUCCEEDED(hr));
		(*List)->SetName(L"CommandList");
	}

	void CommandListManager::DiscardAllocator(uint64_t fenceValueForReset, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& allocator)
	{
		std::lock_guard<std::mutex> LockGuard(m_AllocatorMutex);
		m_DiscardedAllocators.emplace_back(fenceValueForReset, std::move(allocator));
	}

	void CommandListManager::RequestCommandAllocator(ID3D12CommandAllocator** ppAllocator)
	{
		std::lock_guard<std::mutex> LockGuard(m_AllocatorMutex);

		assert((*ppAllocator) == nullptr);
		(*ppAllocator) = nullptr;

		if (!m_DiscardedAllocators.empty())
		{
			auto& allocatorPair = m_DiscardedAllocators.front();

			if (m_Device.GetCommandQueue(m_CmdListType).GetCompletedFenceNum() >= allocatorPair.first)
			{
				*ppAllocator = allocatorPair.second.Detach();
				auto hr = (*ppAllocator)->Reset();

				assert(SUCCEEDED(hr));
				m_DiscardedAllocators.pop_front();
			}
		}

		if ((*ppAllocator) == nullptr)
		{
			auto* pd3d12Device = m_Device.GetD3D12Device();

			auto hr = pd3d12Device->CreateCommandAllocator(
				m_CmdListType,
				IID_PPV_ARGS(ppAllocator));

			(*ppAllocator)->SetName(L"CommandAllocator");
			assert(SUCCEEDED(hr));
		}
	}
}