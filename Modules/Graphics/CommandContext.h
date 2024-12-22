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

		void SetViewports(const D3D12_VIEWPORT* viewports, size_t count) const;
		void SetScissorRects(const D3D12_RECT* scissorRects, size_t count) const;
		void SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvView, BOOL isSingleHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvView) const;

		void UpdateSubresource(ID3D12Resource* dest, ID3D12Resource* intermediate, D3D12_SUBRESOURCE_DATA* pSrcData);

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);
		void FlushResourceBarriers();

		void DiscardAllocator(uint64_t fenceValue);

		D3D12_COMMAND_LIST_TYPE GetType() const;

	private:
		CommandListManager m_CommandListManager;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    m_pCurrentAllocator;

		std::vector<D3D12_RESOURCE_BARRIER> m_PendingResourceBarriers;

	public:
		ID3D12GraphicsCommandList* GetCmdList() const { return m_pCommandList.Get(); }
	};
}