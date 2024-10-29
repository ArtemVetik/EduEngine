#pragma once
#include "pch.h"
#include "CommandListManager.h"

namespace EduEngine
{
	class GRAPHICS_API CommandContext
	{
	public:
		CommandContext(RenderDeviceD3D12& pDevice, D3D12_COMMAND_LIST_TYPE type);
		
		CommandContext(const CommandContext&) = delete;
		CommandContext(CommandContext&&) = delete;
		CommandContext& operator = (const CommandContext&) = delete;
		CommandContext& operator = (CommandContext&&) = delete;

		void Reset();
		ID3D12GraphicsCommandList* Close();

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);
		void FlushResourceBarriers();

		void DiscardAllocator(uint64_t fenceValue);

		D3D12_COMMAND_LIST_TYPE GetType() const;

	private:
		CommandListManager m_CommandListManager;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    m_pCurrentAllocator;

		std::vector<D3D12_RESOURCE_BARRIER> m_PendingResourceBarriers;
	};
}