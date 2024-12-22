#include "pch.h"
#include "CommandContext.h"

namespace EduEngine
{
	CommandContext::CommandContext(RenderDeviceD3D12& pDevice, D3D12_COMMAND_LIST_TYPE type) :
        m_CommandListManager(pDevice, type)
	{
        m_CommandListManager.CreateNewCommandList(m_pCommandList.GetAddressOf(), m_pCurrentAllocator.GetAddressOf());
	}

	void CommandContext::Reset()
	{
        assert(m_pCommandList != nullptr);

        if (!m_pCurrentAllocator)
        {
            m_CommandListManager.RequestCommandAllocator(&m_pCurrentAllocator);
            // Unlike ID3D12CommandAllocator::Reset, ID3D12GraphicsCommandList::Reset can be called while the
            // command list is still being executed. A typical pattern is to submit a command list and then
            // immediately reset it to reuse the allocated memory for another command list.
            m_pCommandList->Reset(m_pCurrentAllocator.Get(), nullptr);
        }
	}

    ID3D12GraphicsCommandList* CommandContext::Close()
    {
        assert(m_pCurrentAllocator != nullptr);
        auto hr = m_pCommandList->Close();
        
        assert(SUCCEEDED(hr));

        return m_pCommandList.Get();
    }

    void CommandContext::SetViewports(const D3D12_VIEWPORT* viewports, size_t count) const
    {
        assert(count < D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
        m_pCommandList->RSSetViewports(count, viewports);
    }

    void CommandContext::SetScissorRects(const D3D12_RECT* scissorRects, size_t count) const
    {
        assert(count < D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
        m_pCommandList->RSSetScissorRects(count, scissorRects);
    }

    void CommandContext::SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvView, BOOL isSingleHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvView) const
    {
        m_pCommandList->OMSetRenderTargets(num, rtvView, isSingleHandle, dsvView);
    }

    void CommandContext::UpdateSubresource(ID3D12Resource* dest, ID3D12Resource* intermediate, D3D12_SUBRESOURCE_DATA* pSrcData)
    {
        UpdateSubresources<1>(m_pCommandList.Get(), dest, intermediate, 0, 0, 1, pSrcData);
    }

    void CommandContext::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier)
    {
        m_PendingResourceBarriers.push_back(barrier);
    }

    void CommandContext::FlushResourceBarriers()
    {
        if (!m_PendingResourceBarriers.empty())
        {
            m_pCommandList->ResourceBarrier(static_cast<UINT>(m_PendingResourceBarriers.size()), m_PendingResourceBarriers.data());
            m_PendingResourceBarriers.clear();
        }
    }

    void CommandContext::DiscardAllocator(uint64_t fenceValue)
    {
        m_CommandListManager.DiscardAllocator(fenceValue, m_pCurrentAllocator);
    }

    D3D12_COMMAND_LIST_TYPE CommandContext::GetType() const
    {
        return m_CommandListManager.GetType();
    }
}