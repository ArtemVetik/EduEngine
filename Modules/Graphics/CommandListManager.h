#pragma once
#include "pch.h"

namespace EduEngine
{
	class GRAPHICS_API RenderDeviceD3D12;

	class GRAPHICS_API CommandListManager
	{
	public:
		CommandListManager(RenderDeviceD3D12& device, const D3D12_COMMAND_LIST_TYPE& listType);

		CommandListManager(const CommandListManager&) = delete;
		CommandListManager(CommandListManager&&) = delete;
		CommandListManager& operator = (const CommandListManager&) = delete;
		CommandListManager& operator = (CommandListManager&&) = delete;

		void CreateNewCommandList(ID3D12GraphicsCommandList** ppList, ID3D12CommandAllocator** ppAllocator);

		void DiscardAllocator(uint64_t fenceValueForReset, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& allocator);
		void RequestCommandAllocator(ID3D12CommandAllocator** ppAllocator);

		D3D12_COMMAND_LIST_TYPE GetType() const { return m_CmdListType; }

	private:
		typedef std::pair<uint64_t, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> DiscardedAllocatorQueueElemType;
		
		std::deque<DiscardedAllocatorQueueElemType> m_DiscardedAllocators;

		const D3D12_COMMAND_LIST_TYPE m_CmdListType;

		std::mutex m_AllocatorMutex;
		RenderDeviceD3D12& m_Device;
	};
}